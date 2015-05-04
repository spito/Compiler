#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

struct BinaryOperator : Expression {
    using Base = Expression;

    BinaryOperator( common::Position p, common::Operator o, EPtr lhs, EPtr rhs ) :
        Base( Kind::BinaryOperator, std::move( p ), o ),
        _left( lhs ),
        _right( rhs )
    {}

    EPtr left() const {
        return _left.get();
    }
    EPtr right() const {
        return _right.get();
    }

private:
    EHandle _left;
    EHandle _right;
};

} // namespace ast
} // namespace compiler
