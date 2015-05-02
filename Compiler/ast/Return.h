#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct Return : Statement< Traversal > {
    using Base = Statement< Traversal >;

    Return( common::Position p, EPtr e ) :
        Base( std::move( p ) ),
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
