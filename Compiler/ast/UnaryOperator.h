#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct UnaryOperator : Expression< Traversal > {
    using Base = Expression< Traversal >;

    UnaryOperator( common::Position p, common::Operator o, EPtr e ) :
        Base( std::move( p ), o ),
        _expression( e )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

    EPtr expression() const {
        return _expression.get();
    }

private:
    EHandle _expression;
};

} // namespace ast
} // namespace compiler
