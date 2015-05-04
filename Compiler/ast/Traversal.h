#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct Traversal {
    virtual ~Traversal() {}

    virtual void eval( const Statement * ) = 0;
};

} // namespace ast
} // namespace compiler
