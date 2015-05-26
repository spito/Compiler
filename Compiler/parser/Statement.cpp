#include "Statement.h"
#include "Expression.h"
#include "StatementExpression.h"

namespace compiler {
namespace parser {

ast::Block *Statement::block() {
    std::unique_ptr< ast::Block > b( new ast::Block );

    auto d = _parser.openBlock( b.get() );

    while ( _it && !_it->isOperator( Operator::BraceClose ) ) {
        auto s = single();
        if ( s )
            b->add( s );
    }
    return b.release();
}

ast::Block *Statement::blockStatement() {
    ++_it;
    std::unique_ptr< ast::Block > b( block() );

    if ( !_it->isOperator( Operator::BraceClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    return b.release();
}

ast::Statement *Statement::single() {

    if ( _it->isOperator( Operator::BraceOpen ) )
        return blockStatement();

    if ( _it->isOperator( Operator::Semicolon ) ) {
        ++_it;
        return nullptr;
    }

    if ( _it->type() == common::Token::Type::Keyword ) {
        switch ( _it->keyword() ) {
        case Keyword::If:
            return ifStatement();
        case Keyword::For:
            return forStatement();
        case Keyword::Do:
            return doWhileStatement();
        case Keyword::While:
            return whileStatement();
        case Keyword::Break:
            return breakStatement();
        case Keyword::Continue:
            return continueStatement();
        case Keyword::Return:
            return returnStatement();
        default:
            break;
        }
    }

    StatementExpression se( _parser, _it );
    std::string variableName;
    //std::unique_ptr< ast::Variable > variable;
    ast::Statement::Handle statement;
    const ast::type::Type *type = nullptr;

    switch ( se.decide() ) {
    case StatementExpression::Type::VariableAssignment:
        variableName = se.variable();
        type = se.type();
    case StatementExpression::Type::Expression:
        statement.reset( se.expression() );
        break;

    case StatementExpression::Type::EmptyExpression:
        if ( !_it->isOperator( Operator::Semicolon ) )
            throw exception::InvalidToken( *_it );
        ++_it;
        return nullptr;
    case StatementExpression::Type::FunctionDefinition:
        // TODO: add warning
        //if ( _parser.insideFunction() )
        //    warning;
    case StatementExpression::Type::FunctionDeclaration:
        return nullptr;
    case StatementExpression::Type::VariableDeclaration:
        variableName = se.variable();
        type = se.type();
        break;
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }

    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    if ( !variableName.empty() )
        _parser.block()->add( variableName, type );

    return statement.release();
}

ast::If *Statement::ifStatement() {
    if ( !_parser.insideFunction() )
        throw exception::InvalidToken( *_it );

    std::unique_ptr< ast::If > stmt( new ast::If( _it->position() ) );

    ++_it;
    if ( !_it->isOperator( Operator::BracketOpen ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    StatementExpression se( _parser, _it );
    ast::Expression::EHandle condition;

    switch ( se.decide( false ) ) {
    case StatementExpression::Type::VariableAssignment:
        stmt->add( se.variable(), se.type() );
    case StatementExpression::Type::Expression:
        condition.reset( se.expression() );
        break;

    case StatementExpression::Type::EmptyExpression:
    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }


    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    ast::Statement::Handle ifPath( single() );
    ast::Statement::Handle elsePath;

    if ( _it->isKeyword( Keyword::Else ) ) {
        ++_it;
        elsePath.reset( single() );
    }

    stmt->assign( condition.release(), ifPath.release(), elsePath.release() );

    return stmt.release();
}

ast::While *Statement::whileStatement() {
    if ( !_parser.insideFunction() )
        throw exception::InvalidToken( *_it );

    std::unique_ptr< ast::While > stmt( new ast::While( _it->position() ) );

    auto d = _parser.openCycle( stmt.get() );

    ++_it;
    if ( !_it->isOperator( Operator::BracketOpen ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    StatementExpression se( _parser, _it );
    ast::Expression::EHandle condition;

    switch ( se.decide( false ) ) {
    case StatementExpression::Type::VariableAssignment:
        stmt->add( se.variable(), se.type() );

    case StatementExpression::Type::Expression:
        condition.reset( se.expression() );
        break;

    case StatementExpression::Type::EmptyExpression:
    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }

    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    ast::Statement::Handle body( single() );

    stmt->assign( condition.release(), body.release() );

    return stmt.release();
}

ast::For *Statement::forStatement() {
    if ( !_parser.insideFunction() )
        throw exception::InvalidToken( *_it );

    std::unique_ptr< ast::For > stmt( new ast::For( _it->position() ) );
    auto d = _parser.openCycle( stmt.get() );

    ++_it;
    if ( !_it->isOperator( Operator::BracketOpen ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    StatementExpression se( _parser, _it );
    ast::Expression::EHandle initialization;
    ast::Expression::EHandle condition;
    ast::Expression::EHandle increment;


    switch ( se.decide( false ) ) {
    case StatementExpression::Type::EmptyExpression:
        break;
    case StatementExpression::Type::VariableAssignment:
        stmt->add( se.variable(), se.type() );
    case StatementExpression::Type::Expression:
        initialization.reset( se.expression() );
        break;

    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }

    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    switch ( se.decide( false ) ) {
    case StatementExpression::Type::EmptyExpression:
        break;
    case StatementExpression::Type::VariableAssignment:
        stmt->add( se.variable(), se.type() );
    case StatementExpression::Type::Expression:
        condition.reset( se.expression() );
        break;

    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }

    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    switch ( se.decide( false ) ) {
    case StatementExpression::Type::EmptyExpression:
        break;
    case StatementExpression::Type::Expression:
        increment.reset( se.expression() );
        break;

    case StatementExpression::Type::VariableAssignment:
    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }


    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    ast::Statement::Handle body( single() );

    stmt->assign( initialization.release(), condition.release(), increment.release(), body.release() );

    return stmt.release();
}

ast::DoWhile *Statement::doWhileStatement() {
    if ( !_parser.insideFunction() )
        throw exception::InvalidToken( *_it );

    std::unique_ptr< ast::DoWhile > stmt( new ast::DoWhile( _it->position() ) );
    auto d = _parser.openCycle( nullptr );

    ++_it;
    ast::Statement::Handle body( single() );

    if ( !_it->isKeyword( Keyword::While ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    if ( !_it->isOperator( Operator::BracketOpen ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    StatementExpression se( _parser, _it );
    ast::Expression::EHandle condition;

    switch ( se.decide( false ) ) {
    case StatementExpression::Type::Expression:
        condition.reset( se.expression() );
        break;

    case StatementExpression::Type::VariableAssignment:
    case StatementExpression::Type::EmptyExpression:
    case StatementExpression::Type::FunctionDeclaration:
    case StatementExpression::Type::FunctionDefinition:
    case StatementExpression::Type::VariableDeclaration:
    case StatementExpression::Type::Void:
        throw exception::InvalidToken( *se.begin() );
    default:
        throw exception::InternalError( "cannot happen" );
    }

    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    stmt->assign( condition.release(), body.release() );

    return stmt.release();
}

ast::Break *Statement::breakStatement() {
    if ( !_parser.insideCycle() )
        throw exception::InvalidToken( *_it );
    std::unique_ptr< ast::Break > s( new ast::Break( _it->position() ) );

    ++_it;
    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;
    return s.release();
}

ast::Continue *Statement::continueStatement() {
    if ( !_parser.insideCycle() )
        throw exception::InvalidToken( *_it );
    std::unique_ptr< ast::Continue > s( new ast::Continue( _it->position() ) );

    ++_it;
    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;
    return s.release();
}

ast::Return *Statement::returnStatement() {
    if ( !_parser.insideFunction() )
        throw exception::InvalidToken( *_it );

    const auto &position = _it->position();

    ++_it;
    if ( _it->isOperator( Operator::Semicolon ) ) {
        ++_it;
        if ( &_parser.function()->returnType() != _parser.tree().typeStorage().fetchType( "void" ) )
            throw exception::InternalError( "mismatched type of return" );
        return new ast::Return( position, nullptr );
    }

    std::unique_ptr< ast::Return > stmt( new ast::Return( position, Expression( _parser, _it ).obtain() ) );
    if ( !_it->isOperator( Operator::Semicolon ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    ExpressionEvaluator ee( _parser );
    ee.start( stmt->expression(), true, true );
    if ( &_parser.function()->returnType() != ee.type() ) {
        if ( ee.type()->kind() == ast::type::Kind::Array )
            throw exception::InternalError( "returning array has no sense" );

        if ( ee.type()->kind() != _parser.function()->returnType().kind() ) {
            // allow assignment of NULL constant
            if ( stmt->expression()->kind() != ast::Kind::Constant || stmt->as< ast::Constant >()->value() != 0 )
                throw exception::InternalError( "mismatched kind of return type" );
        }
    }
    return stmt.release();
}


} // namespace parser
} // namespace compiler
