#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

struct TernaryOperator : Expression {
    using Base = Expression;

    TernaryOperator( common::Position p, EPtr lhs, EPtr mdl, EPtr rhs ) :
        Base( Kind::TernaryOperator, std::move( p ), common::Operator::TernaryOperator ),
        _left( lhs ),
        _middle( mdl ),
        _right( rhs )
    {}

    EPtr left() const {
        return _left.get();
    }
    EPtr middle() const {
        return _middle.get();
    }
    EPtr right() const {
        return _right.get();
    }

private:
    EHandle _left;
    EHandle _middle;
    EHandle _right;
};

} // namespace ast
} // namespace compiler
