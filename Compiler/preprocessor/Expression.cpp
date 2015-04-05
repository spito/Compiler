#include "Expression.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace preprocessor {

//using Token = common::Token;
using Type = common::Token::Type;
using Operator = common::Operator;
using Handle = std::unique_ptr < Expression > ;

static int rank( Operator op ) {
    switch ( op ) {
    case Operator::BracketOpen:
        return 0;
    case Operator::LogicalNot:
        return 1;
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
        return 2;
    case Operator::Addition:
    case Operator::Subtraction:
        return 3;
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
        return 4;
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
        return 5;
    case Operator::EqualTo:
    case Operator::NotEqualTo:
        return 6;
    case Operator::BitwiseAnd:
        return 7;
    case Operator::BitwiseXor:
        return 8;
    case Operator::BitwiseOr:
        return 9;
    case Operator::LogicalAnd:
        return 10;
    case Operator::LogicalOr:
        return 11;
    case Operator::None:
        return 12;
    default:
        throw exception::InternalError( "unrankable operator" );
    }
}

enum class Precedence : bool {
    LeftToRight,
    RightToLeft
};

static Precedence precedence( Operator op ) {
    switch ( op ) {
    case Operator::LogicalNot:
        return Precedence::RightToLeft;
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
    case Operator::Addition:
    case Operator::Subtraction:
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
    case Operator::EqualTo:
    case Operator::NotEqualTo:
    case Operator::BitwiseAnd:
    case Operator::BitwiseXor:
    case Operator::BitwiseOr:
    case Operator::LogicalAnd:
    case Operator::LogicalOr:
        return Precedence::LeftToRight;
    default:
        throw exception::InternalError( "unprecedencable operator" );
    }
}

void Expression::prepare( Iterator begin, Iterator end ) {

    for ( ; begin != end; ++begin ) {
        if ( begin->type() != Type::Operator )
            continue;

        switch ( begin->op() ) {
        case Operator::Ampersand: begin->op() = Operator::BitwiseAnd; break;
        case Operator::Star: begin->op() = Operator::Multiplication; break;
        case Operator::Plus: begin->op() = Operator::Addition; break;
        case Operator::Minus: begin->op() = Operator::Subtraction; break;
        default:;
        }
    }
}

Expression::Value Expression::value() {
    switch ( _op ) {
    case Operator::LogicalNot:
        return !right().value();
    case Operator::Multiplication:
        return left().value() * right().value();
    case Operator::Division:
        return left().value() / right().value();
    case Operator::Modulo:
        return left().value() % right().value();
    case Operator::Addition:
        return left().value() + right().value();
    case Operator::Subtraction:
        return left().value() - right().value();
    case Operator::BitwiseLeftShift:
        return left().value() << right().value();
    case Operator::BitwiseRightShift:
        return left().value() >> right().value();
    case Operator::LessThan:
        return left().value() < right().value();
    case Operator::LessThenOrEqual:
        return left().value() <= right().value();
    case Operator::GreaterThan:
        return left().value() > right().value();
    case Operator::GreaterThanOrEqual:
        return left().value() >= right().value();
    case Operator::EqualTo:
        return left().value() == right().value();
    case Operator::NotEqualTo:
        return left().value() != right().value();
    case Operator::BitwiseAnd:
        return Unsigned( left().value() ) & Unsigned( right().value() );
    case Operator::BitwiseXor:
        return Unsigned( left().value() ) ^ Unsigned( right().value() );
    case Operator::BitwiseOr:
        return Unsigned( left().value() ) | Unsigned( right().value() );
    case Operator::LogicalAnd:
        return left().value() && right().value();
    case Operator::LogicalOr:
        return left().value() || right().value();
    case Operator::None:
        return _value;
    default:
        throw exception::InternalError( "expression has invalid operator" );
    }
}

bool Expression::isCloser( Operator op ) const {
    Operator my = _op == Operator::None ? _owner : _op;

    if ( rank( my ) == rank( op ) )
        return precedence( my ) == Precedence::RightToLeft;
    return rank( my ) > rank( op );
}
bool Expression::isFarther( Operator op ) const {
    Operator my = _op == Operator::None ? _owner : _op;

    if ( rank( my ) == rank( op ) )
        return precedence( my ) == Precedence::LeftToRight;
    return rank( my ) < rank( op );
}

bool Expression::isUnary() const {
    switch ( _op ) {
    case Operator::LogicalNot:
        return true;
    default:
        return false;
    }
}
bool Expression::isPrefix() const {
    switch ( _op ) {
    case Operator::LogicalNot:
        return true;
    default:
        return false;
    }
}
bool Expression::isPostfix() const {
    return false;
}
bool Expression::isBinary() const {
    switch ( _op ) {
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
    case Operator::Addition:
    case Operator::Subtraction:
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
    case Operator::EqualTo:
    case Operator::NotEqualTo:
    case Operator::BitwiseAnd:
    case Operator::BitwiseXor:
    case Operator::BitwiseOr:
    case Operator::LogicalAnd:
    case Operator::LogicalOr:
        return true;
    default:
        return false;
    }
}

Expression::Expression( IteratorPack &iterator, Side side, Operator owner ) : Expression( owner ) {

    if ( !iterator )
        throw exception::InternalError( "expression is corrupted" );

    switch ( iterator->type() ) {
    case Type::Char:
        _value = iterator->value().front();
        _op = Operator::None;
        ++iterator;
        break;
    case Type::Integer:
        _value = iterator->integer();
        _op = Operator::None;
        ++iterator;
        break;
    case Type::Real:
        _value = Value( iterator->real() );
        _op = Operator::None;
        ++iterator;
        break;
    case Type::Operator:
        switch ( iterator->op() ) {
        case Operator::LogicalNot:
            _op = Operator::LogicalNot;
            ++iterator;
            right() = Expression( iterator, Side::Right, _op );
            break;
        case Operator::BracketOpen:
            ++iterator;
            *this = Expression( iterator );
            if ( !iterator || !iterator->isOperator( Operator::BracketClose ) )
                throw exception::InvalidToken( *iterator );
            break;
        default:
            throw exception::InvalidToken( *iterator );
        }
        ++iterator;
        break;
    default:
        throw exception::InvalidToken( *iterator );
    }

    while ( iterator ) {

        if ( iterator->type() != Type::Operator )
            throw exception::InvalidToken( *iterator );

        if ( iterator->op() == Operator::BracketClose )
            break;

        Expression top;
        top._op = iterator->op();

        if ( isFarther( top._op ) && side == Side::Right )
            return;

        ++iterator;
        top.left() = std::move( *this );
        top.right() = Expression( iterator, Side::Right, top._op );
        *this = std::move( top );
    }

}



} // namespace preprocessor
} // namespace compiler

