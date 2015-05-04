#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct Break : Statement {
    using Base = Statement;

    Break( common::Position p ) :
        Base( Kind::Break, std::move( p ) )
    {}

};

} // namespace ast
} // namespace compiler
