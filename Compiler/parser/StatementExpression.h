#pragma once

#include "Parser.h"
#include "SmartIterator.h"

namespace compiler {
namespace parser {

struct StatementExpression {

    using Operator = common::Operator;

    enum class Type {
        EmptyExpression,
        Void,
        Expression,
        VariableAssignment,
        VariableDeclaration,
        FunctionDefinition,
        FunctionDeclaration,
    };

    StatementExpression( Parser &p, SmartIterator &it ) :
        _it( it ),
        _begin( it ),
        _parser( p )
    {}

    Type decide( bool = true );
    SmartIterator begin() const {
        return _begin;
    }
    SmartIterator end() const {
        return _it;
    }

    const ast::Function *function() {
        return _function;
    }
    const std::string &variable() {
        return _variableName;
    }
    ast::Expression *expression() {
        return _expression.release();
    }
    const ast::type::Type *type() const {
        return _type;
    }

private:

    SmartIterator &_it;
    SmartIterator _begin;
    Parser &_parser;

    const ast::Function *_function = nullptr;
    std::string _variableName;
    std::unique_ptr< ast::Expression > _expression;
    const ast::type::Type *_type = nullptr;
};

} // namespace parser
} // namespace compiler
