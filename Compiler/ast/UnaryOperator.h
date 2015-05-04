#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

struct UnaryOperator : Expression {
    using Base = Expression;

    UnaryOperator( common::Position p, common::Operator o, EPtr e ) :
        Base( Kind::UnaryOperator, std::move( p ), o ),
        _expression( e )
    {}

    EPtr expression() const {
        return _expression.get();
    }

private:
    EHandle _expression;
};

} // namespace ast
} // namespace compiler
