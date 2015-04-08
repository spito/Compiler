#include "Expression.h"
#include "../includes/exceptions.h"

#include <iostream>

namespace compiler {
namespace preprocessor {

using Type = common::Token::Type;
using Operator = common::Operator;
using Value = long long;
using Unsigned = unsigned long long;
using SmartIterator = common::SmartIterator < std::vector< common::Token >::iterator > ;

static int precedenceRank( Operator op ) {
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

static Precedence asociativity( Operator op ) {
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

void solveAmbiguity( SmartIterator iterator ) {
    if ( !iterator || iterator->type() != Type::Operator )
        return;
    switch ( iterator->op() ) {
    case Operator::Ampersand: iterator->op() = Operator::BitwiseAnd; break;
    case Operator::Star: iterator->op() = Operator::Multiplication; break;
    case Operator::Plus: iterator->op() = Operator::Addition; break;
    case Operator::Minus: iterator->op() = Operator::Subtraction; break;
    default:;
    }
}

static Value eval( Operator op, Value value, Value lhs, Value rhs ) {
    switch ( op ) {
    case Operator::LogicalNot:
        return !rhs;
    case Operator::Multiplication:
        return lhs * rhs;
    case Operator::Division:
        return lhs / rhs;
    case Operator::Modulo:
        return lhs % rhs;
    case Operator::Addition:
        return lhs + rhs;
    case Operator::Subtraction:
        return lhs - rhs;
    case Operator::BitwiseLeftShift:
        return lhs << rhs;
    case Operator::BitwiseRightShift:
        return lhs >> rhs;
    case Operator::LessThan:
        return lhs < rhs;
    case Operator::LessThenOrEqual:
        return lhs <= rhs;
    case Operator::GreaterThan:
        return lhs > rhs;
    case Operator::GreaterThanOrEqual:
        return lhs >= rhs;
    case Operator::EqualTo:
        return lhs == rhs;
    case Operator::NotEqualTo:
        return lhs != rhs;
    case Operator::BitwiseAnd:
        return Unsigned( lhs ) & Unsigned( rhs );
    case Operator::BitwiseXor:
        return Unsigned( lhs ) ^ Unsigned( rhs );
    case Operator::BitwiseOr:
        return Unsigned( lhs ) | Unsigned( rhs );
    case Operator::LogicalAnd:
        return lhs && rhs;
    case Operator::LogicalOr:
        return lhs || rhs;
    case Operator::None:
        return value;
    default:
        throw exception::InternalError( "expression has invalid operator" );
    }
}

static bool isCloser( Operator owner, Operator foreign ) {

    if ( precedenceRank( owner ) == precedenceRank( foreign ) )
        return asociativity( owner ) == Precedence::RightToLeft;
    return precedenceRank( owner ) > precedenceRank( foreign );
}
static bool isFarther( Operator owner, Operator foreign ) {

    if ( precedenceRank( owner ) == precedenceRank( foreign ) )
        return asociativity( owner ) == Precedence::LeftToRight;
    return precedenceRank( owner ) < precedenceRank( foreign );
}

static bool isUnary( Operator op ) {
    switch ( op ) {
    case Operator::LogicalNot:
        return true;
    default:
        return false;
    }
}
static bool isPrefix( Operator op ) {
    switch ( op ) {
    case Operator::LogicalNot:
        return true;
    default:
        return false;
    }
}
static bool isPostfix( Operator op ) {
    return false;
}
static bool isBinary( Operator op ) {
    switch ( op ) {
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

enum class Side : bool {
    Left,
    Right
};

static Value expression( SmartIterator &iterator, Side side, Operator owner ) {

    solveAmbiguity( iterator );

    Value value = 0, left = 0, right = 0;
    Operator self = Operator::None;

    if ( !iterator )
        throw exception::InternalError( "expression is corrupted" );

    switch ( iterator->type() ) {
    case Type::Char:
        value = iterator->value().front();
        ++iterator;
        break;
    case Type::Integer:
        value = iterator->integer();
        ++iterator;
        break;
    case Type::Real:
        value = Value( iterator->real() );
        ++iterator;
        break;
    case Type::Operator:
        switch ( iterator->op() ) {
        case Operator::LogicalNot:
            self = Operator::LogicalNot;
            ++iterator;
            right = expression( iterator, Side::Right, self );
            break;
        case Operator::BracketOpen:
            ++iterator;
            value = expression( iterator, Side::Left, Operator::None );
            if ( !iterator || !iterator->isOperator( Operator::BracketClose ) )
                throw exception::InvalidToken( *iterator );
            ++iterator;
            break;
        default:
            throw exception::InvalidToken( *iterator );
        }
        break;
    default:
        throw exception::InvalidToken( *iterator );
    }
    while ( iterator ) {

        if ( iterator->type() != Type::Operator )
            throw exception::InvalidToken( *iterator );

        if ( iterator->op() == Operator::BracketClose )
            break;

        solveAmbiguity( iterator );
        Operator next = iterator->op();

        if ( isFarther( owner, next ) && side == Side::Right )
            break;

        ++iterator;
        left = eval( self, value, left, right );
        self = next;
        right = expression( iterator, Side::Right, self );
    }
    return eval( self, value, left, right );
}

bool expression( SmartIterator &&iterator ) {
    return expression( iterator, Side::Left, Operator::None ) != 0;
}

} // namespace preprocessor
} // namespace compiler

