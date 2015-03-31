#include "Preprocessor.h"
#include "Substituer.h"
#include "ShadowChunker.h"
#include "../common/InputBuffer.h"

#include <fstream>

namespace compiler {
namespace preprocessor {

void Preprocessor::saveToFile( const std::string &name ) const {
    std::ofstream out( name.c_str() );

    out << content();
}


Preprocessor::Preprocessor( const std::string &name ) :
    _tokenizer( std::ifstream( name.c_str() ) ),
    _symbols( std::make_shared< common::SymbolTable< Symbol > >() )
{
    setPredefined();
    removeComments(); // phase 1
    processText(); // phase 2
}

void Preprocessor::nextPhase() {
    _tokenizer.restart( _content );
    _content.clear();
}

void Preprocessor::removeComments() {

    common::Token token;

    while ( true ) {
        _tokenizer.readToken( token );

        if ( token.type() == common::Token::Type::Eof )
            break;

        if ( token.type() != common::Token::Type::Comment ) {
            _content += token.token();
            continue;
        }

        for ( char c : token.token() ) {
            if ( common::isNewLine( c ) )
                _content += c;
            else
                _content += ' ';
        }
    }
    nextPhase();
}

void Preprocessor::processText() {

    common::Token token;

    bool quit = false;

    while ( !quit ) {
        _tokenizer.readToken( token );

        switch ( token.type() ) {
        case common::Token::Type::Word:
        {
            Substituer substituer( _symbols );
            ShadowChunker chunker( token.token(), [&] {
                common::Token token;
                _tokenizer.readToken( token );
                return token.token();
            } );
            substituer.run( chunker );
            _content += substituer.result();
        }
        break;
        case common::Token::Type::Space:
            _content += token.token();
            break;
        case common::Token::Type::Operator:
            if ( token.token() == "#" ) {
                if ( _ready ) {
                    invokeExpression();
                    break;
                }
                else
                    throw exception::InvalidToken( token );
            }
            if ( token.token() == "##" )
                throw exception::InvalidToken( token );
        case common::Token::Type::Char:
        case common::Token::Type::Integer:
        case common::Token::Type::Real:
        case common::Token::Type::String:
        case common::Token::Type::StringInclude:
            _content += token.token();
            break;
        case common::Token::Type::Eof:
            quit = true;
            continue;
        default:
            throw exception::InvalidToken( token );
        }
        _ready = token.containsNewLine();
    }

}

void Preprocessor::invokeExpression() {
    common::Token token;
    common::Position before;
    do {
        before = position();
        _tokenizer.readToken( token );
    } while ( token.type() == common::Token::Type::Space );

    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    if ( token.token() == "define" )
        processDefine();
    else if ( token.token() == "pragma" )
        processPragma();
    else if ( token.token() == "undef" )
        processUndef();
    else
        throw exception::InvalidCharacterConstant( token.token(), before );
}

void Preprocessor::includeFile( std::string )
{
}

void Preprocessor::includeSystemFile( std::string )
{
}

void Preprocessor::processDefine() {

    common::Token token;
    std::string name;
    std::vector< std::string > value;
    std::vector< std::string > params;
    bool isInteger = false;
    int integer;

    common::Position before = position();

    _tokenizer.readToken( token );
    if ( token.type() != common::Token::Type::Space )
        throw exception::InvalidToken( token );

    _tokenizer.readToken( token );
    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    Symbol symbol( token.token() );

    _tokenizer.readToken( token );

    // parse formal parameters
    if ( token.type() == common::Token::Type::Operator && token.token() == "(" ) {

        while ( true ) {

            _tokenizer.readToken( token );
            if ( token.type() == common::Token::Type::Space )
                _tokenizer.readToken( token );

            if ( token.type() != common::Token::Type::Word )
                throw exception::InvalidToken( token );
            params.push_back( token.token() );

            _tokenizer.readToken( token );
            if ( token.type() == common::Token::Type::Space )
                _tokenizer.readToken( token );
            
            if ( token.type() == common::Token::Type::Operator ) {
                if ( token.token() == "," )
                    continue;
                if ( token.token() == ")" )
                    break;
            }
            throw exception::InvalidToken( token );
        }
        _tokenizer.readToken( token );
    }

    if ( token.type() != common::Token::Type::Space )
        throw exception::InvalidToken( token );

    while ( !token.containsNewLine() ) {
        _tokenizer.lookAtToken( token );

        if ( token.containsNewLine() )
            break;
        _tokenizer.readToken( token );

        isInteger = value.empty() && token.type() == common::Token::Type::Integer;
        if ( isInteger )
            integer = int( token.integer() );
        value.push_back( token.token() );
    }

    if ( isInteger )
        symbol = Symbol( name, integer );
    else if ( !params.empty() )
        symbol = Symbol( name, params, value );
    else if ( !value.empty() )
        symbol = Symbol( name, value );

    if ( auto fetched = _symbols->find( symbol ) ) {
        if ( !symbol.identical( *fetched ) )
            throw exception::InternalError( "Redefinition of symbol", before );
    }
    _symbols->insert( std::move( symbol ) );
}

void Preprocessor::processUndef() {
    common::Token token;
    std::string name;

    _tokenizer.readToken( token );
    if ( token.type() != common::Token::Type::Space )
        throw exception::InvalidToken( token );

    _tokenizer.readToken( token );
    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    name = token.token();

    _tokenizer.lookAtToken( token );
    if ( !token.containsNewLine() )
        throw exception::InvalidToken( token );

    _symbols->remove( name );
}

void Preprocessor::processPragma() {}

void Preprocessor::setPredefined() {
}

} // namespace preprocessor
} // namespace compiler


