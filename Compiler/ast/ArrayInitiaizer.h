#pragma once

#include "Expression.h"
#include "Variable.h"

#include <vector>
#include <numeric>

namespace compiler {
namespace ast {

struct ArrayInitializer : Expression {
    using Base = Expression;

    ArrayInitializer( common::Position p, Variable *v, std::vector< int > d, std::vector< EHandle > values ) :
        Base( Kind::ArrayInitializer, std::move( p ), common::Operator::None ),
        _variable( v ),
        _dimensions( std::move( d ) ),
        _values( std::move( values ) )
    {}

    const std::vector< EHandle > &values() const {
        return _values;
    }

    const std::vector< int > &dimensions() const {
        return _dimensions;
    }

    const Variable *variable() const {
        return _variable.get();
    }

private:
    std::unique_ptr< Variable > _variable;
    std::vector< int > _dimensions;
    std::vector< EHandle > _values;
    
};

} // namespace ast
} // namespace compiler
