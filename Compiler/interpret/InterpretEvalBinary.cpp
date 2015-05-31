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

void Interpret::eval( const ast::BinaryOperator *e ) {
    eval( e->left() );

    common::Register l = _info->load();

    switch ( e->op() ) {
    case common::Operator::LogicalAnd:
        if ( l.zero() ) {
            _info->remember( common::Register( 0 ) );
            return;
        }
        break;
    case common::Operator::LogicalOr:
        if ( !l.zero() ) {
            _info->remember( common::Register( 1 ) );
            return;
        }
        break;
    default:
        break;
    }

    auto left = _info.get();
    eval( e->right() );
    common::Register r = _info->load();

    switch ( e->op() ) {
    case common::Operator::ArrayAccess:
        r.clearMess();
        switch ( left->type()->kind() ) {
        case ast::type::Kind::Array:
        case ast::type::Kind::Pointer:
            _info = new Information( left->variable()[ r.getu64() ] );
            addRegister();
            break;
        case ast::type::Kind::Elementary:
            throw exception::InternalError( "cannot use array access operator to elementary" );
        }
        break;
    case common::Operator::TypeCast:
        typeCast( _info.get(), left->type() );
        break;
    case common::Operator::Multiplication:
        _info->remember( l *= r );
        break;
    case common::Operator::Division:
        _info->remember( l /= r );
        break;
    case common::Operator::Modulo:
        _info->remember( l %= r );
        break;
    case common::Operator::Addition:
        _info->remember( l += r );
        break;
    case common::Operator::Subtraction:
        _info->remember( l -= r );
        break;
    case common::Operator::BitwiseLeftShift:
        _info->remember( l <<= r );
        break;
    case common::Operator::BitwiseRightShift:
        _info->remember( l >>= r );
        break;
    case common::Operator::LessThan:
        _info->remember( l < r );
        break;
    case common::Operator::LessThenOrEqual:
        _info->remember( l <= r );
        break;
    case common::Operator::GreaterThan:
        _info->remember( l > r );
        break;
    case common::Operator::GreaterThanOrEqual:
        _info->remember( l >= r );
        break;
    case common::Operator::EqualTo:
        _info->remember( l == r );
        break;
    case common::Operator::NotEqualTo:
        _info->remember( l != r );
        break;
    case common::Operator::BitwiseAnd:
        _info->remember( l &= r );
        break;
    case common::Operator::BitwiseXor:
        _info->remember( l ^= r );
        break;
    case common::Operator::BitwiseOr:
        _info->remember( l |= r );
        break;
    case common::Operator::LogicalAnd:
    case common::Operator::LogicalOr:
        _info->remember( common::Register( int( !r.zero() ) ) );
        break;

    case common::Operator::Assignment:
        left->store( r );
        _info->remember( r );
        break;
    case common::Operator::AssignmentProduct:
        left->store( l *= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentQuotient:
        left->store( l /= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentRemainder:
        left->store( l %= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentSum:
        left->store( l += r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentDifference:
        left->store( l -= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentLeftShift:
        left->store( l <<= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentRightShift:
        left->store( l >>= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentBitwiseAnd:
        left->store( l &= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentBitwiseXor:
        left->store( l ^= r );
        _info->remember( l );
        break;
    case common::Operator::AssignmentBitwiseOr:
        left->store( l |= r );
        _info->remember( l );
        break;
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

} // namespace interpret
} // namespace compiler
