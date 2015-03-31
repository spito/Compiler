#include "Tokenizer.h"
#include "../common/utils.h"
#include "../common/CharClass.h"
#include "../common/NumberParser.h"

namespace compiler {
namespace preprocessor {

bool Tokenizer::isTokenType( common::Token &token, const common::Token::Type type ) {
    common::Position before = position();
    getToken( token );
    if ( token.type() == type )
        return true;

    position( before );
    return false;
}

void Tokenizer::readToken( common::Token &token, Class ignore ) {
    getToken( token, ignore );
}

void Tokenizer::lookAtToken( common::Token &token, Class ignore ) {
    common::Position before = position();
    getToken( token, ignore );
    position( before );
}

void Tokenizer::getToken( common::Token &token, Class ignore ) {
    common::Position before = position();

    if ( _input.eof() ) {
        token = common::Token( common::Token::Type::Eof, before );
        return;
    }
    Class tClass = resolveClass( ignore );
    switch ( tClass ) {
    case Class::Preprocessor:
        processSharp( token );
        break;
    case Class::Slash: /// TODO: inspect again
        processSlash( token );
        break;
    case Class::String:
        processString( token );
        break;
    case Class::Number:
        processNumber( token );
        break;
    case Class::CompoundOperator: // can fall down
        if ( processCompoundOperator( token ) )
            break;
    case Class::SingleOperator:// stop falling
        processSingleOperator( token );
        break;
    case Class::Space:
        processSpace( token );
        break;
    case Class::ShortComment:
        processShortComment( token );
        break;
    case Class::LongComment:
        processLongComment( token );
        break;
    case Class::Word:
        processWord( token );
        break;
    default:
        throw exception::InvalidCharacter( _input.look(), before );
    }

}

Tokenizer::Class Tokenizer::resolveClass( Class ignore ) {
    char c = _input.look();
    char next = _input.look( 1 );

    if ( ignore != Class::Preprocessor && common::isPreprocessorSign( c ) )
        return Class::Preprocessor;
    if ( ignore != Class::Slash && common::isSlash( c ) )
        return Class::Slash;
    if ( ignore != Class::Space && common::isSpace( c ) )
        return Class::Space;
    if ( ignore != Class::ShortComment && common::isShortComment( c, next ) )
        return Class::ShortComment;
    if ( ignore != Class::LongComment && common::isLongComment( c, next ) )
        return Class::LongComment;
    if ( ignore != Class::CompoundOperator && common::isCompoundOperatorBegin( c ) )
        return Class::CompoundOperator;
    if ( ignore != Class::SingleOperator && common::isSingleOperator( c ) )
        return Class::SingleOperator;
    if ( ignore != Class::Number && common::isNumber( c ) )
        return Class::Number;
    if ( ignore != Class::Word && common::isWordBegin( c ) )
        return Class::Word;
    if ( ignore != Class::String && common::isString( c ) )
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
    token = common::Token( common::Token::Type::Word, rawToken, before );
}

void Tokenizer::processSpace( common::Token &token ) {
    std::string rawToken;
    bool newLine = false;

    common::Position before = position();

    while ( !_input.eof() ) {
        if ( !common::isSpace( _input.look() ) )
            break;
        char c = _input.read();
        if ( common::isNewLine( c ) )
            newLine = true;

        rawToken += c;
    }
    token = common::Token( common::Token::Type::Space, rawToken, before );
    token.containsNewLine() = newLine;
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
        token = common::Token( common::Token::Type::String, rawToken, before );
    else if ( quots == '\'' ) {
        if ( rawToken.size() != 1 )
            throw exception::InvalidCharacterConstant( rawToken, before );
        token = common::Token( common::Token::Type::Char, rawToken, before );
    }
    else if ( quots == '<' )
        token = common::Token( common::Token::Type::StringInclude, rawToken, before );
    else
        throw exception::InvalidCharacter( quots, before );
}

void Tokenizer::processShortComment( common::Token &token ) {
    std::string rawToken;
    bool slash = false;

    common::Position before = position();

    while ( !_input.eof() ) {
        char c = _input.look();
        if ( slash )
            slash = false;
        else if ( common::isSlash( c ) )
            slash = true;
        else if ( common::isNewLine( c ) )
            break;
        rawToken += _input.read();
    }
    token = common::Token( common::Token::Type::Comment, rawToken, before );
}

void Tokenizer::processLongComment( common::Token &token ) {
    std::string rawToken;
    bool star = false;
    bool newLine = false;

    common::Position before = position();

    while ( true ) {

        char c = _input.read();

        if ( _input.eof() )
            throw exception::EndOfFile( "", position() );

        if ( common::isNewLine( c ) )
            newLine = true;

        rawToken += c;

        if ( star && c == '/' )
            break;
        else if ( c == '*' )
            star = true;
        else
            star = false;
    }
    token = common::Token( common::Token::Type::Comment, rawToken, before );
    token.containsNewLine() = newLine;
}

void Tokenizer::processSingleOperator( common::Token &token ) {
    std::string rawToken;
    common::Position before = position();

    rawToken += _input.read();

    token = common::Token( common::Token::Type::Operator, rawToken, before );
}

bool Tokenizer::processCompoundOperator( common::Token &token ) {
    std::string rawToken;
    common::Position before = position();

    rawToken += _input.look();

    for ( int i = 1; common::isCompounfOperatorLater( _input.look( 1 ) ); ++i )
        rawToken += _input.look( i );

    if ( rawToken.size() == 1 )
        return false;

    token = common::Token( common::Token::Type::Operator, rawToken, before );
    for ( int i = 0; i < int( rawToken.size() ); ++i )
        _input.read();
    return true;
}

void Tokenizer::processSlash( common::Token &token ) {
    common::Position before = position();

    _input.read(); // erase slash \ 

    if ( common::isNewLine( _input.look() ) ) {
        _input.read(); // erase \n
        token = common::Token( common::Token::Type::Space, " ", before );
        return;
    }
    token = common::Token( common::Token::Type::Word, "\\", before );
}

void Tokenizer::processSharp( common::Token &token ) {
    common::Position before = position();

    std::string rawToken;

    while ( common::isPreprocessorSign( _input.look() ) ) {
        rawToken += _input.read();
    }
    if ( rawToken.empty() || rawToken.size() > 2 )
        throw exception::InvalidCharacterConstant( rawToken, before );

    token = common::Token( common::Token::Type::Operator, rawToken, before );
}


} // namespace preprocessor
} // namespace compiler