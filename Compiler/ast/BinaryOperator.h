#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct BinaryOperator : Expression< Traversal > {
    using Base = Expression< Traversal >;

    BinaryOperator( common::Position p, common::Operator o, EPtr lhs, EPtr rhs ) :
        Base( std::move( p ), o ),
        _left( lhs ),
        _right( rhs )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

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
