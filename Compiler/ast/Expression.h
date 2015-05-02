#pragma once

#include "Statement.h"
#include "../common/Operator.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct Expression : Statement< Traversal > {

    Expression( common::Operator op ) :
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
