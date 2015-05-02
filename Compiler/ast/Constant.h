#pragma once

#include "Expression.h"

#include <string>

namespace compiler {
namespace ast {

template< typename Traversal >
struct Constant : Expression< Traversal > {
    using Base = Expression< Traversal >;

    Constant( common::Position p, long long value ) :
        Base( std::move( p ), common::Operator::None ),
        _value( value )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

    long long value() const {
        return _value;
    }

private:
    long long _value;
};

} // namespace ast
} // namespace compiler
