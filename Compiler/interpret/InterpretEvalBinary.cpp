#include "Interpret.h"

namespace compiler {
namespace interpret {

static void typeCast( Information &toStore, const ast::TypeOf &targetType ) {
    common::Register r = toStore.load();
    bool sign;
    bool ptr;
    int length = targetType.bytes();
    switch ( targetType.kind() ) {
    case ast::TypeOf::Kind::Array:
        throw exception::InternalError( "cannot cast to array" );
    case ast::TypeOf::Kind::Elementary:
        ptr = false;
        sign = targetType.isSigned();
        break;
    case ast::TypeOf::Kind::Pointer:
        ptr = true;
        sign = targetType.of()->isSigned();
        break;
    }
    r.castTo( ptr, length, sign );
    toStore = Information( r, targetType );
}

Information Interpret::eval( const ast::BinaryOperator *e ) {
    Information left = eval( e->left() );
    common::Register l = left.load();

    switch ( e->op() ) {
    case common::Operator::LogicalAnd:
        if ( l.zero() ) {
            return Information( common::Register( 0 ), ast::TypeStorage::type( "int" ) );
        }
        break;
    case common::Operator::LogicalOr:
        if ( !l.zero() ) {
            return Information( common::Register( 1 ), ast::TypeStorage::type( "int" ) );
        }
        break;
    default:
        break;
    }

    Information right = eval( e->right() );
    common::Register r = right.load();

    switch ( e->op() ) {
    case common::Operator::ArrayAccess:
        r.clearMess();
        switch ( left.type().kind() ) {
        case ast::TypeOf::Kind::Array:
        case ast::TypeOf::Kind::Pointer:
            return Information( left.variable()[ r.getu64() ] );
        case ast::TypeOf::Kind::Elementary:
            throw exception::InternalError( "cannot use array access operator to elementary" );
        default:
            throw exception::InternalError( "ice: unknown type" );
        }
        break;
    case common::Operator::TypeCast:
        typeCast( right, left.type() );
        return right;
    case common::Operator::Multiplication:
        right.remember( l *= r );
        return right;
    case common::Operator::Division:
        right.remember( l /= r );
        return right;
    case common::Operator::Modulo:
        right.remember( l %= r );
        return right;
    case common::Operator::Addition:
        right.remember( l += r );
        return right;
    case common::Operator::Subtraction:
        right.remember( l -= r );
        return right;
    case common::Operator::BitwiseLeftShift:
        right.remember( l <<= r );
        return right;
    case common::Operator::BitwiseRightShift:
        right.remember( l >>= r );
        return right;
    case common::Operator::LessThan:
        right.remember( l < r );
        return right;
    case common::Operator::LessThenOrEqual:
        right.remember( l <= r );
        return right;
    case common::Operator::GreaterThan:
        right.remember( l > r );
        return right;
    case common::Operator::GreaterThanOrEqual:
        right.remember( l >= r );
        return right;
    case common::Operator::EqualTo:
        right.remember( l == r );
        return right;
    case common::Operator::NotEqualTo:
        right.remember( l != r );
        return right;
    case common::Operator::BitwiseAnd:
        right.remember( l &= r );
        return right;
    case common::Operator::BitwiseXor:
        right.remember( l ^= r );
        return right;
    case common::Operator::BitwiseOr:
        right.remember( l |= r );
        return right;
    case common::Operator::LogicalAnd:
    case common::Operator::LogicalOr:
        right.remember( common::Register( int( !r.zero() ) ) );
        return right;

    case common::Operator::Initialization:
    case common::Operator::Assignment:
        left.store( r );
        return left;
    case common::Operator::AssignmentProduct:
        left.store( l *= r );
        return left;
    case common::Operator::AssignmentQuotient:
        left.store( l /= r );
        return left;
    case common::Operator::AssignmentRemainder:
        left.store( l %= r );
        return left;
    case common::Operator::AssignmentSum:
        left.store( l += r );
        return left;
    case common::Operator::AssignmentDifference:
        left.store( l -= r );
        return left;
    case common::Operator::AssignmentLeftShift:
        left.store( l <<= r );
        return left;
    case common::Operator::AssignmentRightShift:
        left.store( l >>= r );
        return left;
    case common::Operator::AssignmentBitwiseAnd:
        left.store( l &= r );
        return left;
    case common::Operator::AssignmentBitwiseXor:
        left.store( l ^= r );
        return left;
    case common::Operator::AssignmentBitwiseOr:
        left.store( l |= r );
        return left;
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

} // namespace interpret
} // namespace compiler
