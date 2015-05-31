#include "ExpressionEvaluator.h"

namespace compiler {
namespace parser {

void ExpressionEvaluator::eval( const ast::Statement *s ) {
    if ( !valid() )
        return;

    switch ( s->kind() ) {
    case ast::Kind::Constant:
        return eval( s->as< ast::Constant >() );
    case ast::Kind::Variable:
        if ( !_typeOnly ) {
            _failed = true;
            break;
        }
        return eval( s->as< ast::Variable >() );
    case ast::Kind::UnaryOperator:
        return eval( s->as< ast::UnaryOperator >() );
    case ast::Kind::BinaryOperator:
        return eval( s->as< ast::BinaryOperator >() );
    case ast::Kind::TernaryOperator:
        return eval( s->as< ast::TernaryOperator >() );
    case ast::Kind::Call:
        if ( !_typeOnly ) {
            _failed = true;
            break;
        }
        return eval( s->as< ast::Call >() );
    default:
        _failed = true;
    }

}

void ExpressionEvaluator::eval( const ast::Constant *e ) {
    _value = common::Register( int( e->value() ) );
    _type = e->type();
}

void ExpressionEvaluator::eval( const ast::Variable *e ) {
    auto v = _parser.getVariable( e->name() );
    if ( !v )
        throw exception::InternalError( "undefined variable" );
    _type = v->type();
}

void ExpressionEvaluator::eval( const ast::UnaryOperator *e ) {
    using Operator = common::Operator;

    eval( e->expression() );
    if ( !valid() )
        return;

    switch ( e->op() ) {
    case Operator::LogicalNot:
        !_value;
        break;
    case Operator::BitwiseNot:
        ~_value;
        break;
    case Operator::Ampersand:
    case Operator::AddressOf:
        if ( _type.count() == 0 )
            throw exception::InternalError( "invalid ast tree" );
        _type = ast::ProxyType( _type ).pointer();
        break;
    case Operator::Star:
    case Operator::Dereference:
        if ( _type.of() )
            _type = *_type.of();
        else
            throw exception::InternalError( "invalid ast tree" );
        break;
    case Operator::PrefixIncrement:
    case Operator::PrefixDecrement:
    case Operator::SuffixIncrement:
    case Operator::SuffixDecrement:
        if ( !_typeOnly )
            _failed = true;
        break;
    case Operator::Plus:
        +_value;
        deduceType();
        break;
    case Operator::Minus:
        -_value;
        deduceType();
        break;
    case Operator::Sizeof:
        _value.setu64( _type.bytes() );
        break;
    default:
        throw exception::InternalError( "invalid ast tree" );
    }
}

void ExpressionEvaluator::eval( const ast::BinaryOperator *e ) {
    using Operator = common::Operator;

    eval( e->left() );
    if ( !valid() )
        return;

    switch ( e->op() ) {
    case common::Operator::LogicalAnd:
        if ( _value.zero() ) {
            _value = common::Register( 0 );
            deduceType();
            return;
        }
        break;
    case common::Operator::LogicalOr:
        if ( !_value.zero() ) {
            _value = common::Register( 1 );
            deduceType();
            return;
        }
        break;
    default:
        break;
    }

    auto leftValue = _value;
    ast::TypeOf leftType = _type;

    eval( e->right() );
    if ( !valid() )
        return;

    switch ( e->op() ) {
    case common::Operator::ArrayAccess:
        if ( !_typeOnly )
            _failed = true;
        else if ( leftType.of() )
            _type = *leftType.of();
        else
            throw exception::InternalError( "invalid ast tree" );
        break;
    case common::Operator::TypeCast:
        _value.clearMess();
        _type = leftType;
        break;
    case common::Operator::Multiplication:
        _value = leftValue *= _value;
        deduceType();
        break;
    case common::Operator::Division:
        _value = leftValue /= _value;
        deduceType();
        break;
    case common::Operator::Modulo:
        _value = leftValue %= _value;
        deduceType();
        break;
    case common::Operator::Addition:
        _value = leftValue += _value;
        deduceType();
        break;
    case common::Operator::Subtraction:
        _value = leftValue -= _value;
        deduceType();
        break;
    case common::Operator::BitwiseLeftShift:
        _value = leftValue <<= _value;
        deduceType();
        break;
    case common::Operator::BitwiseRightShift:
        _value = leftValue >>= _value;
        deduceType();
        break;
    case common::Operator::LessThan:
        _value = leftValue < _value;
        deduceType();
        break;
    case common::Operator::LessThenOrEqual:
        _value = leftValue <= _value;
        deduceType();
        break;
    case common::Operator::GreaterThan:
        _value = leftValue > _value;
        deduceType();
        break;
    case common::Operator::GreaterThanOrEqual:
        _value = leftValue >= _value;
        deduceType();
        break;
    case common::Operator::EqualTo:
        _value = leftValue == _value;
        deduceType();
        break;
    case common::Operator::NotEqualTo:
        _value = leftValue != _value;
        deduceType();
        break;
    case common::Operator::BitwiseAnd:
        _value = leftValue &= _value;
        deduceType();
        break;
    case common::Operator::BitwiseXor:
        _value = leftValue ^= _value;
        deduceType();
        break;
    case common::Operator::BitwiseOr:
        _value = leftValue |= _value;
        deduceType();
        break;
    case common::Operator::LogicalAnd:
    case common::Operator::LogicalOr:
        _value = common::Register( int( !_value.zero() ) );
        deduceType();
        break;

    case common::Operator::Assignment:
    case common::Operator::AssignmentProduct:
    case common::Operator::AssignmentQuotient:
    case common::Operator::AssignmentRemainder:
    case common::Operator::AssignmentSum:
    case common::Operator::AssignmentDifference:
    case common::Operator::AssignmentLeftShift:
    case common::Operator::AssignmentRightShift:
    case common::Operator::AssignmentBitwiseAnd:
    case common::Operator::AssignmentBitwiseXor:
    case common::Operator::AssignmentBitwiseOr:
        if ( !_typeOnly )
            _failed = true;
        else
            _type = leftType;
        break;
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

void ExpressionEvaluator::eval( const ast::TernaryOperator *e ) {

    if ( _typeOnly ) {
        eval( e->middle() );
        return;
    }

    eval( e->left() );
    if ( !valid() )
        return;

    if ( !_value.zero() )
        eval( e->middle() );
    else
        eval( e->right() );
}

void ExpressionEvaluator::eval( const ast::Call *e ) {
    _type = _parser.tree().findFunction( e->name() ).returnType();
}


} // namespace parser
} // namespace compiler
