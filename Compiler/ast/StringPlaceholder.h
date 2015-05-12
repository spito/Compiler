#pragma once

#include "Expression.h"

namespace compiler {
namespace ast {

struct StringPlaceholder : Expression {
    using Base = Expression;

    StringPlaceholder( common::Position p, std::string value ) :
        Base( Kind::StringPlaceholder, std::move( p ), common::Operator::None )
    {}

    const std::string &value() const {
        return _value;
    }

private:

    std::string _value;
};

} // namespace ast
} // namespace compiler
