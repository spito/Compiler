#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct TernaryOperator : Expression< Traversal > {
    using Base = Expression< Traversal >;

    TernaryOperator( common::Position p, common::Operator o, EPtr lhs, EPtr mdl, EPtr rhs ) :
        Base( std::move( p ), o ),
        _left( lhs ),
        _middle( mdl ),
        _right( rhs )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

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
