#pragma once

#include "Expression.h"

#include <string>

namespace compiler {
namespace ast {

template< typename Traversal >
struct Variable : Expression< Traversal > {
    using Base = Expression< Traversal >;

    Variable( common::Position p, std::string n ) :
        Base( std::move( p ), common::Operator::None ),
        _name( std::move( n ) )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

    const std::string &name() const {
        return _name;
    }

private:
    std::string _name;
};

} // namespace ast
} // namespace compiler
