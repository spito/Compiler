#include "ExpressionEvaluator.h"

namespace compiler {
namespace parser {

void ExpressionEvaluator::eval( const ast::Constant *e ) {
    _value = common::Register( int( e->value() ) );
    _type = e->type();
}

void ExpressionEvaluator::eval( const ast::Variable *e ) {
    auto v = _parser.getVariable( e->name() );
    if ( !v )
        throw exception::InternalError( "undefined variable" );
    _type = &v->type();
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
        if ( !_type )
            throw exception::InternalError( "invalid ast tree" );
        _type = _parser.typeStorage().addType< ast::type::Pointer >( _type );
        break;
    case Operator::Star:
        if ( !_type )
            throw exception::InternalError( "invalid ast tree" );
        if ( _type->kind() == ast::type::Kind::Array )
            _type = &_type->as< ast::type::Array >()->of();
        else if ( _type->kind() == ast::type::Kind::Pointer )
            _type = &_type->as< ast::type::Pointer >()->of();
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
        _value.setu64( _type->size() );
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
    }

    auto leftValue = _value;
    const auto *leftType = _type;

    eval( e->right() );
    if ( !valid() )
        return;

    switch ( e->op() ) {
    case common::Operator::ArrayAccess:
        if ( !_typeOnly )
            _failed = true;
        else if ( leftType->kind() == ast::type::Kind::Array )
            _type = &leftType->as< ast::type::Array >()->of();
        else if ( leftType->kind() == ast::type::Kind::Pointer )
            _type = &leftType->as< ast::type::Pointer >()->of();
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
    _type = &_parser.tree().findFunction( e->name() )->returnType();
}


} // namespace parser
} // namespace compiler
