#pragma once

#include "Expression.h"

#include <string>

namespace compiler {
namespace ast {

struct Variable : Expression {
    using Base = Expression;

    Variable( common::Position p, std::string n ) :
        Base( Kind::Variable, std::move( p ), common::Operator::None ),
        _name( std::move( n ) )
    {}

    const std::string &name() const {
        return _name;
    }

private:
    std::string _name;
};

} // namespace ast
} // namespace compiler
