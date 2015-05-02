#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct Break : Statement< Traversal > {
    using Base = Statement< Traversal >;

    Break( common::Position p ) :
        Base( std::move( p ) )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }
};

} // namespace ast
} // namespace compiler
