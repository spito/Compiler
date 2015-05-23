#include "StatementExpression.h"
#include "Declaration.h"
#include "Expression.h"

namespace compiler {
namespace parser {

auto StatementExpression::decide( bool fullExpression ) -> Type {
    Declaration d( _parser, _it );
    std::unique_ptr< ast::Variable > variable;

    if ( _it->isOperator( Operator::Semicolon ) || _it->isOperator( Operator::BracketClose ) )
        return Type::EmptyExpression;

    switch ( d.decide( fullExpression ) ) {
    case Declaration::Type::None:
        _expression.reset( Expression( _parser, _it ) );
        return Type::Expression;

    case Declaration::Type::TypeOnly:
    case Declaration::Type::Void:
    case Declaration::Type::VariadicPack:
        throw exception::InvalidToken( *_begin );

    case Declaration::Type::SingleVariable:
        variable.reset( d.variable() );
        _variableName = variable->name();
        _type = d.typeOnly();

        if ( _it->isOperator( Operator::Assignment ) ) {
            const auto &position = _it->position();
            ++_it;

            switch ( d.typeOnly()->kind() ) {
            case ast::type::Kind::Array:
                // TODO: implement
                return Type::VariableDeclaration;
            case ast::type::Kind::Elementary:
            case ast::type::Kind::Pointer:
                _expression.reset( new ast::BinaryOperator(
                    position,
                    Operator::Assignment,
                    variable.release(),
                    Expression( _parser, _it )
                    ) );
                break;
            default:
                throw exception::InternalError( "cannot happen" );
            }

            return Type::VariableAssignment;
        }
        else
            return Type::VariableDeclaration;

    case Declaration::Type::Function:
        _function = d.function();
        return _function->definition() ?
            Type::FunctionDefinition :
            Type::FunctionDeclaration;

    default:
        throw exception::InternalError( "cannot happen" );
    }
}

} // namespace parser
} // namespace compiler
