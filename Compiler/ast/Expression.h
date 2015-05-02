#pragma once

#include "Statement.h"
#include "../common/Operator.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct Expression : Statement< Traversal > {
    using Base = Statement< Traversal >;


    Expression( common::Position p, common::Operator op ) :
        Base( std::move( p ) ),
        _op( op )
    {}

    common::Operator op() const {
        return _op;
    }

private:
    common::Operator _op;
};

} // namespace ast
} // namespace compiler
