#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct Continue : Statement {
    using Base = Statement;

    Continue( common::Position p ) :
        Base( Kind::Continue, std::move( p ) )
    {}

};

} // namespace ast
} // namespace compiler
