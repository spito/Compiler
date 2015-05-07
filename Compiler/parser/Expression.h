#pragma once

#include "Parser.h"
#include "ExpressionEvaluator.h"
#include "../ast/Expression.h"
#include "../common/TokenStore.h"

namespace compiler {
namespace parser {

struct Expression {

    using Operator = common::Operator;
    using SmartIterator = common::TokenStore::Iterator;

    Expression( Parser &p, SmartIterator &it ) :
        _it( it ),
        _parser( p ),
        _evaluator( p )
    {}

    operator ast::Expression *( ) {
        return descend( Side::Left, Operator::None );
    }

private:

    enum class Side : bool {
        Left,
        Right
    };

    ast::Expression *descend( Side, Operator );
    ast::Expression *sizeofExpression();
    ast::Expression *functionCall( std::string );

    bool leaving();
    bool unary();
    bool binary();
    bool ternary();
    bool nnary();

    void solvePrefixAmbiguity();
    void solvePostfixAmbiguity();

    SmartIterator &_it;
    Parser &_parser;

    ExpressionEvaluator _evaluator;

    bool _functionArguments = false;
    bool _sizeofProcessing = false;
};
} // namespace parser
} // namespace compiler
