#include "OperatorParser.h"

namespace compiler {
namespace preprocessor {

void OperatorParser::store( const char *value, common::Operator op ) {
    _value = value;
    _kind = op;
}

void OperatorParser::stage1() {
    using common::Operator;

    common::Position before = _buffer.position();

    char c = _buffer.read();

    switch ( c ) {
    case '(': store( "(", Operator::BracketOpen ); break;
    case ')': store( ")", Operator::BracketClose ); break;
    case '{': store( "{", Operator::BraceOpen ); break;
    case '}': store( "}", Operator::BraceClose ); break;
    case '[': store( "[", Operator::BracketIndexOpen ); break;
    case ']': store( "]", Operator::BracketIndexClose ); break;
    case '~': store( "~", Operator::BitwiseNot ); break;
    case '?': store( "?", Operator::QuestionMark ); break;
    case ':': store( ":", Operator::Colon ); break;
    case ';': store( ";", Operator::Semicolon ); break;
    case ',': store( ",", Operator::Comma ); break;

    case '+':
        if ( !stage2( c ) )
            store( "+", Operator::Plus );
        break;
    case '-':
        if ( !stage2( c ) )
            store( "-", Operator::Minus );
        break;
    case '*':
        if ( !stage2( c ) )
            store( "*", Operator::Star );
        break;
    case '/':
        if ( !stage2( c ) )
            store( "/", Operator::Division );
        break;
    case '%':
        if ( !stage2( c ) )
            store( "%", Operator::Modulo );
        break;
    case '&':
        if ( !stage2( c ) )
            store( "&", Operator::Ampersand );
        break;
    case '^':
        if ( !stage2( c ) )
            store( "^", Operator::BitwiseXor );
        break;
    case '|':
        if ( !stage2( c ) )
            store( "|", Operator::BitwiseOr );
        break;
    case '!':
        if ( !stage2( c ) )
            store( "!", Operator::LogicalNot );
        break;
    case '<':
        if ( !stage2( c ) )
            store( "<", Operator::LessThan );
        break;
    case '>':
        if ( !stage2( c ) )
            store( ">", Operator::GreaterThan );
        break;
    case '=':
        if ( !stage2( c ) )
            store( "=", Operator::Assignment );
        break;

    case '.':
        stage2( c );
        break;
    default:
        throw exception::InvalidCharacter( c, "(){}[]~?:;,+-*/%&^|!<>=.", before );
    }
}

bool OperatorParser::stage2( char prev ) {
    using common::Operator;

    char c = _buffer.look();

    switch ( prev ) {
    case '+':
        switch ( c ) {
        case '+': store( "++", Operator::Increment ); break;
        case '=': store( "+=", Operator::AssignmentSum ); break;
        default: return false;
        }
        break;
    case '-':
        switch ( c ) {
        case '-': store( "--", Operator::Decrement ); break;
        case '=': store( "-=", Operator::AssignmentDifference ); break;
        default: return false;
        }
        break;
    case '*':
        switch ( c ) {
        case '=': store( "*=", Operator::AssignmentProduct ); break;
        default: return false;
        }
        break;
    case '/':
        switch ( c ) {
        case '=': store( "/=", Operator::AssignmentQuotient ); break;
        default: return false;
        }
        break;
    case '%':
        switch ( c ) {
        case '=': store( "%=", Operator::AssignmentRemainder ); break;
        default: return false;
        }
        break;
    case '&':
        switch ( c ) {
        case '&': store( "&&", Operator::LogicalAnd ); break;
        case '=': store( "&=", Operator::AssignmentBitwiseAnd ); break;
        default: return false;
        }
        break;
    case '^':
        switch ( c ) {
        case '=': store( "^=", Operator::AssignmentBitwiseXor ); break;
        default: return false;
        }
        break;
    case '|':
        switch ( c ) {
        case '|': store( "||", Operator::LogicalOr ); break;
        case '=': store( "|=", Operator::AssignmentBitwiseOr ); break;
        default: return false;
        }
        break;
    case '!':
        switch ( c ) {
        case '=': store( "!=", Operator::NotEqualTo ); break;
        default: return false;
        }
        break;
    case '<':
        switch ( c ) {
        case '=': store( "<=", Operator::LessThenOrEqual ); break;
        case '<':
            if ( !stage3( prev, c ) )
                store( "<<", Operator::BitwiseLeftShift );
            break;
        default: return false;
        }
        break;
    case '>':
        switch ( c ) {
        case '=': store( ">=", Operator::GreaterThanOrEqual ); break;
        case '>':
            if ( !stage3( prev, c ) )
                store( ">>", Operator::BitwiseRightShift );
            break;
        default: return false;
        }
        break;
    case '=':
        switch ( c ) {
        case '=': store( "==", Operator::EqualTo ); break;
        default: return false;
        }
        break;
    case '.':
        if ( c == '.' ) {
            stage3( prev, c );
            break;
        }
        throw exception::InvalidCharacter( c, '.' );
    default:
        throw exception::InternalError( "Not reachable path." );
    }

    _buffer.read();
    return true;
}

bool OperatorParser::stage3( char, char prev ) {
    using common::Operator;
    char c = _buffer.look( 1 );

    switch ( prev ) {
    case '<':
        switch ( c ) {
        case '=': store( "<<=", Operator::AssignmentLeftShift ); break;
        default: return false;
        }
        break;
    case '>':
        switch ( c ) {
        case '=': store( ">>=", Operator::AssignmentRightShift ); break;
        default: return false;
        }
        break;
    case '.':
        if ( c == '.' ) {
            store( "...", Operator::VariadicArgument );
            break;
        }
        throw exception::InvalidCharacter( c, '.' );
    default:
        throw exception::InternalError( "Not reachable path." );
    }
    _buffer.read();
    return true;

}

} // namespace preprocessor
} // namespace compiler
