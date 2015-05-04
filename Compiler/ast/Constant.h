#pragma once

#include "Expression.h"
#include "Type.h"

#include <string>

namespace compiler {
namespace ast {

struct Constant : Expression {
    using Base = Expression;

    Constant( common::Position p, long long value, const type::Type *type ) :
        Base( Kind::Constant, std::move( p ), common::Operator::None ),
        _value( value ),
        _type( type )
    {}

    long long value() const {
        return _value;
    }

    const type::Type *type() const {
        return _type;
    }

private:
    long long _value;
    const type::Type *_type;
};

} // namespace ast
} // namespace compiler
