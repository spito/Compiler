#include "Tokenizer.h"
#include "../common/utils.h"
#include "../common/CharClass.h"
#include "../common/NumberParser.h"

namespace compiler {
namespace preprocessor {

bool Tokenizer::isTokenType( common::Token &token, const common::Token::Type type ) {
    common::Position before = position();
    getToken( token, true );
    if ( token.type() == type )
        return true;

    position( before );
    return false;
}

void Tokenizer::readToken( common::Token &token, bool acceptSpaces ) {
    getToken( token, acceptSpaces );
}

void Tokenizer::lookAtToken( common::Token &token, bool acceptSpaces ) {
    common::Position before = position();
    getToken( token, acceptSpaces );
    position( before );
}

void Tokenizer::getToken( common::Token &token, bool acceptSpaces ) {
    common::Position before = position();

    if ( _input.eof() ) {
        token = common::Token( common::Token::Type::Eof, before );
        return;
    }
    bool quit = false;
    while ( !quit ) {
        quit = true;

        Class tClass = resolveClass();
        switch ( tClass ) {
        case Class::Preprocessor:
            processSharp( token );
            break;
        case Class::Slash: /// TODO: inspect again
            processSlash();
            quit = false;
            break;
        case Class::String:
            processString( token );
            break;
        case Class::Number:
            processNumber( token );
            break;
        case Class::Operator:
            processOperator( token );
            break;
        case Class::NewLine:
            processNewLine( token );
            break;
        case Class::Space:
            processSpace( token );
            quit = acceptSpaces;
            break;
        case Class::ShortComment:
            processShortComment();
            quit = false;
            break;
        case Class::LongComment:
            processLongComment();
            quit = false;
            break;
        case Class::Word:
            processWord( token );
            break;
        default:
            throw exception::InvalidCharacter( _input.look(), before );
        }
    }

}

Tokenizer::Class Tokenizer::resolveClass() {
    char c = _input.look();
    char next = _input.look( 1 );

    if ( common::isPreprocessorSign( c ) )
        return Class::Preprocessor;
    if ( common::isSlash( c ) )
        return Class::Slash;
    if ( common::isNewLine( c ) )
        return Class::NewLine;
    if ( common::isSpace( c ) )
        return Class::Space;
    if ( common::isShortComment( c, next ) )
        return Class::ShortComment;
    if ( common::isLongComment( c, next ) )
        return Class::LongComment;
    if ( common::isOperator( c ) )
        return Class::Operator;
    if ( common::isNumber( c ) )
        return Class::Number;
    if ( common::isWordBegin( c ) )
        return Class::Word;
    if ( common::isString( c ) )
        return Class::String;
    return Class::None;
}

void Tokenizer::processNumber( common::Token &token ) {
    common::Position before = position();
    common::NumberParser p( _input );

    p.run();

    if ( p.isReal() ) {
        token = common::Token( common::Token::Type::Real, p.token(), before );
        token.real() = p.real();
    }
    else {
        token = common::Token( common::Token::Type::Integer, p.token(), before );
        token.integer() = p.integer();
    }
}

void Tokenizer::processWord( common::Token &token ) {
    std::string rawToken;

    common::Position before = position();

    while ( true ) {
        if ( !common::isWordLater( _input.look() ) )
            break;

        rawToken += _input.read();
    }
    token = common::Token( common::Token::Type::Word, std::move( rawToken ), before );
}

void Tokenizer::processSpace( common::Token &token ) {
    std::string rawToken;
    common::Position before = position();

    while ( !_input.eof() ) {
        char c = _input.look();
        if ( !common::isSpace( c ) || common::isNewLine( c ) )
            break;
        rawToken += _input.read();
    }
    token = common::Token( common::Token::Type::Space, std::move( rawToken ), before );
}

void Tokenizer::processString( common::Token &token ) {
    std::string rawToken;
    bool special = false;

    common::Position before = position();
    char quots = _input.read();// read "
    char endQuots = quots;
    if ( endQuots == '<' )
        endQuots = '>';

    while ( true ) {
        if ( _input.eof() ) {
            throw exception::EndOfFile( "", position() );
        }
        common::Position p = position();
        char c = _input.read();

        if ( special ) {
            switch ( c ) {
            case '"':
            case '\'':
            case '\\':
            case '/':
                rawToken.push_back( c );
                break;
            case 'b':
                rawToken.push_back( '\b' );
                break;
            case 'f':
                rawToken.push_back( '\f' );
                break;
            case 'n':
                rawToken.push_back( '\n' );
                break;
            case 'r':
                rawToken.push_back( '\r' );
                break;
            case 't':
                rawToken.push_back( '\t' );
                break;
            case '\n':
                rawToken.push_back( '\n' );
                break;
            default:
                throw exception::InvalidCharacter( c, "\"\\/bfnrt", p );
            }
            special = false;
        }
        else if ( common::isSlash( c ) )
            special = true;
        else if ( common::isNewLine( c ) )
            throw exception::InvalidCharacter( c, position() );
        else if ( c == endQuots )
            break;
        else
            rawToken.push_back( c );
    }
    if ( quots == '"' )
        token = common::Token( common::Token::Type::String, std::move( rawToken ), before );
    else if ( quots == '\'' ) {
        if ( rawToken.size() != 1 )
            throw exception::InvalidCharacterConstant( std::move( rawToken ), before );
        token = common::Token( common::Token::Type::Char, std::move( rawToken ), before );
    }
    else if ( quots == '<' )
        token = common::Token( common::Token::Type::StringInclude, std::move( rawToken ), before );
    else
        throw exception::InvalidCharacter( quots, before );
}

void Tokenizer::processShortComment() {
    bool slash = false;

    common::Position before = position();

    while ( !_input.eof() ) {
        char c = _input.look();

        if ( common::isSlash( c ) && common::isNewLine( _input.look( 1 ) ) ) {
            _input.read();
            _input.read();
            continue;
        }


        if ( common::isNewLine( c ) )
            break;
        _input.read();
    }
}

void Tokenizer::processLongComment() {
    bool star = false;

    common::Position before = position();

    while ( true ) {

        char c = _input.read();

        if ( _input.eof() )
            throw exception::EndOfFile( "", position() );

        if ( common::isSlash( c ) && common::isNewLine( _input.look( 1 ) ) ) {
            _input.read();
            continue;
        }

        if ( star && c == '/' )
            break;
        else if ( c == '*' )
            star = true;
        else
            star = false;
    }
}

void Tokenizer::processOperator( common::Token &token ) {
    common::Position before = position();
    std::string rawToken;
    rawToken += _input.read();
    token = common::Token( common::Token::Type::Operator, std::move( rawToken ), before );
}


void Tokenizer::processSlash() {

    _input.read(); // erase slash \ 

    if ( common::isNewLine( _input.look() ) ) {
        _input.read(); // erase \n
        return;
    }
    throw exception::InvalidCharacter( _input.look(), '\n' );
}

void Tokenizer::processSharp( common::Token &token ) {
    common::Position before = position();

    std::string rawToken;

    while ( common::isPreprocessorSign( _input.look() ) ) {
        rawToken += _input.read();
    }
    if ( rawToken.empty() || rawToken.size() > 2 )
        throw exception::InvalidCharacterConstant( std::move( rawToken ), before );

    token = common::Token( common::Token::Type::Operator, std::move( rawToken ), before );
}

void Tokenizer::processNewLine( common::Token &token ) {
    token = common::Token( common::Token::Type::NewLine, "\n", position() );
    _input.read();
}


} // namespace preprocessor
} // namespace compiler