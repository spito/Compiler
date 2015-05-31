#include "StatementExpression.h"
#include "Declaration.h"
#include "Expression.h"
#include "ArrayInitializer.h"

namespace compiler {
namespace parser {

auto StatementExpression::decide( bool fullExpression ) -> Type {
    Declaration d( _parser, _it );
    std::unique_ptr< ast::Variable > variable;

    if ( _it->isOperator( Operator::Semicolon ) || _it->isOperator( Operator::BracketClose ) )
        return Type::EmptyExpression;

    switch ( d.decide( fullExpression ) ) {
    case Declaration::Type::None:
        _expression.reset( Expression( _parser, _it ).obtain() );
        return Type::Expression;

    case Declaration::Type::TypeOnly:
    case Declaration::Type::Void:
    case Declaration::Type::VariadicPack:
        throw exception::InvalidToken( *_begin );

    case Declaration::Type::SingleVariable:
        variable.reset( d.variable() );
        _variableName = variable->name();
        _type = d.type();

        if ( _it->isOperator( Operator::Assignment ) ) {
            const auto &position = _it->position();
            ++_it;

            switch ( d.type().kind() ) {
            case ast::TypeOf::Kind::Array:
                _expression.reset(
                    ArrayInitializer( _parser, _it ).obtain( variable.release(), _type )
                    );
                break;
            case ast::TypeOf::Kind::Elementary:
            case ast::TypeOf::Kind::Pointer:
                _expression.reset( new ast::BinaryOperator(
                    position,
                    Operator::Initialization,
                    variable.release(),
                    Expression( _parser, _it ).obtain()
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
