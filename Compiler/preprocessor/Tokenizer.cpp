#include "../common/utils.h"
#include "../common/CharClass.h"
#include "Tokenizer.h"
#include "NumberParser.h"
#include "OperatorParser.h"

namespace compiler {
namespace preprocessor {

common::Token Tokenizer::readToken( bool acceptSpaces ) {
    return getToken( acceptSpaces );
}

void Tokenizer::giveBack( const common::Token &token ) {
    position( token.position() );
}

common::Token Tokenizer::getToken( bool acceptSpaces ) {
    common::Position before = position();

    while ( true ) {
        
        Class tClass = resolveClass();
        switch ( tClass ) {
        case Class::Preprocessor:
            return processSharp();
            break;
        case Class::Slash:
            processSlash();
            break;
        case Class::String:
            return processString();
        case Class::Number:
            return processNumber();
        case Class::Operator:
            return processOperator();
        case Class::NewLine:
            return processNewLine();
        case Class::Space:
            if ( acceptSpaces )
                return processSpace();
            processSpace();
            break;
        case Class::ShortComment:
            processShortComment();
            break;
        case Class::LongComment:
            processLongComment();
            break;
        case Class::Word:
            return processWord();
        case Class::Eof:
            return common::Token( common::Token::Type::Eof, before );
        default:
            throw exception::InvalidCharacter( _input.look(), before );
        }
    }

}

Tokenizer::Class Tokenizer::resolveClass() {
    if ( _input.eof() )
        return Class::Eof;

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

common::Token Tokenizer::processNumber() {
    common::Position before = position();
    NumberParser p( _input );

    common::Token token( std::move( p.value() ),
                         p.isReal() ?
                         common::Token::Type::Real :
                         common::Token::Type::Integer,
                         before );
    if ( p.isReal() )
        token.real() = p.real();
    else
        token.integer() = p.integer();
    return token;
}

common::Token Tokenizer::processOperator() {
    common::Position before = position();

    OperatorParser p( _input );

    common::Token token( std::move( p.value() ), common::Token::Type::Operator, before );
    token.op() = p.op();
    return token;
}


common::Token Tokenizer::processWord() {
    std::string rawToken;

    common::Position before = position();

    while ( true ) {

        if ( common::isSlash( _input.look() ) && common::isNewLine( _input.look( 1 ) ) ) {
            _input.read();
            _input.read();
            continue;
        }

        if ( !common::isWordLater( _input.look() ) )
            break;

        rawToken += _input.read();
    }
    return common::Token( std::move( rawToken ), common::Token::Type::Word, before );
}

common::Token Tokenizer::processSpace() {
    std::string rawToken;
    common::Position before = position();

    while ( !_input.eof() ) {
        char c = _input.look();
        if ( !common::isSpace( c ) || common::isNewLine( c ) )
            break;
        rawToken += _input.read();
    }
    return common::Token( std::move( rawToken ), common::Token::Type::Space, before );
}

common::Token Tokenizer::processString() {
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
            case '\\':
            case '"':
            case '\'':
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
            case '\n': // just skip this newline
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
        return common::Token( std::move( rawToken ), common::Token::Type::String, before );
    if ( quots == '\'' ) {
        if ( rawToken.size() != 1 )
            throw exception::InvalidCharacterConstant( std::move( rawToken ), before );
        return common::Token( std::move( rawToken ), common::Token::Type::Char, before );
    }
    if ( quots == '<' )
        return common::Token( std::move( rawToken ), common::Token::Type::StringInclude, before );

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
        if ( _input.eof() )
            throw exception::EndOfFile( "", position() );

        char c = _input.read();

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

void Tokenizer::processSlash() {

    _input.read(); // erase slash \ 

    if ( common::isNewLine( _input.look() ) ) {
        _input.read(); // erase \n
        return;
    }
    throw exception::InvalidCharacter( _input.look(), '\n' );
}

common::Token Tokenizer::processSharp() {
    common::Position before = position();

    std::string rawToken;

    while ( common::isPreprocessorSign( _input.look() ) ) {
        rawToken += _input.read();
    }
    if ( rawToken.empty() || rawToken.size() > 2 )
        throw exception::InvalidCharacterConstant( std::move( rawToken ), before );

    common::Token token( std::move( rawToken ), common::Token::Type::Operator, before );
    token.op() =
        token.value().size() == 1 ?
        common::Operator::Sharp :
        common::Operator::TwoShaprs;
    return token;
}

common::Token Tokenizer::processNewLine() {
    common::Position before = position();
    _input.read();
    return common::Token( "\n", common::Token::Type::NewLine, before );
}


} // namespace preprocessor
} // namespace compiler
