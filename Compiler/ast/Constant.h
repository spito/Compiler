#pragma once

#include "Expression.h"
#include "Type.h"

#include <string>

namespace compiler {
namespace ast {

struct Constant : Expression {
    using Base = Expression;

    Constant( common::Position p, long long value, TypeOf type ) :
        Base( Kind::Constant, std::move( p ), common::Operator::None ),
        _value( value ),
        _type( std::move( type ) )
    {
        if ( !value )
            _type = TypeOf::makeNull( _type );
    }

    long long value() const {
        return _value;
    }

    const TypeOf &type() const {
        return _type;
    }

private:
    long long _value;
    TypeOf _type;
};

} // namespace ast
} // namespace compiler
