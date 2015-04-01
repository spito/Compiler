#include "Preprocessor.h"
#include "Substituer.h"
#include "ShadowChunker.h"
#include "../common/InputBuffer.h"

#include <fstream>

namespace compiler {
namespace preprocessor {

Preprocessor Preprocessor::start( const std::string &name ) {
    context::file( name );
    return Preprocessor( name );
}
    
Preprocessor::Preprocessor( const std::string &name ) :
    _tokenizer( std::ifstream( name.c_str() ) ),
    _symbols( std::make_shared< common::SymbolTable< Symbol > >() ),
    _name( &context::file() )
{
    context::position( &position() );
    setPredefined();
    processText();
}

void Preprocessor::processText() {

    common::Token token;

    bool quit = false;
    _store.push( common::Token( common::Token::Type::FileBegin ) );
    while ( !quit ) {
        _tokenizer.readToken( token );
        switch ( token.type() ) {
        case common::Token::Type::NewLine:
            _ready = true;
            break;
        case common::Token::Type::Word:
        {
            Substituer substituer( _symbols );
            ShadowChunker chunker( token, [&] {
                common::Token token;
                _tokenizer.readToken( token, true );
                return token;
            } );
            substituer.run( chunker, token.position() );
            _store.push( substituer.result() );
            _ready = false;
        }
            break;
        case common::Token::Type::Operator:
            if ( token.value() == "#" ) {
                if ( _ready ) {
                    _ready = false;
                    invokeExpression();
                    break;
                }
                else
                    throw exception::InvalidToken( token );
            }
            if ( token.value() == "##" )
                throw exception::InvalidToken( token );
        case common::Token::Type::Char:
        case common::Token::Type::Integer:
        case common::Token::Type::Real:
        case common::Token::Type::String:
        case common::Token::Type::StringInclude:
            _store.push( std::move( token ) );
            _ready = false;
            break;
        case common::Token::Type::Eof:
            _store.push( common::Token( common::Token::Type::FileEnd ) );
            quit = true;
            continue;
        default:
            throw exception::InvalidToken( token );
        }
    }

}

void Preprocessor::invokeExpression() {
    common::Token token;
    common::Position before;

    _tokenizer.readToken( token );

    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    if ( token.value() == "define" )
        processDefine();
    else if ( token.value() == "pragma" )
        processPragma();
    else if ( token.value() == "undef" )
        processUndef();
    else
        throw exception::InvalidCharacterConstant( token.value(), before );
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
    std::vector< common::Token > value;
    std::vector< common::Token > params;

    common::Position before = position();

    _tokenizer.readToken( token, true );
    if ( token.type() != common::Token::Type::Space )
        throw exception::InvalidToken( token );

    _tokenizer.readToken( token );
    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    Symbol symbol( token.value() );

    _tokenizer.lookAtToken( token );

    // parse formal parameters
    if ( token.type() == common::Token::Type::Operator && token.value() == "(" ) {
        _tokenizer.readToken( token );
        while ( true ) {

            _tokenizer.readToken( token );

            if ( token.type() != common::Token::Type::Word )
                throw exception::InvalidToken( token );
            params.push_back( token );

            _tokenizer.readToken( token );

            if ( token.type() == common::Token::Type::Operator ) {
                if ( token.value() == "," )
                    continue;
                if ( token.value() == ")" )
                    break;
            }
            throw exception::InvalidToken( token );
        }
    }

    while ( true ) {
        _tokenizer.lookAtToken( token );

        if ( token.type() == common::Token::Type::NewLine )
            break;
        _tokenizer.readToken( token );

        value.push_back( token );
    }

    if ( value.size() == 1 && value.back().type() == common::Token::Type::Integer )
        symbol = Symbol( name, value.back() );
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
    if ( token.type() != common::Token::Type::Word )
        throw exception::InvalidToken( token );

    name = token.value();

    _tokenizer.lookAtToken( token );
    if ( token.type() != common::Token::Type::NewLine )
        throw exception::InvalidToken( token );

    _symbols->remove( name );
}

void Preprocessor::processPragma() {}

void Preprocessor::setPredefined() {
}

} // namespace preprocessor
} // namespace compiler


