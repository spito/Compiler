#pragma once

#include "Expression.h"

#include <vector>
#include <string>
#include <algorithm>

namespace compiler {
namespace ast {

struct Call : Expression {
    using Base = Expression;

    Call( common::Position p, std::string name, std::vector< EHandle > &&params ) :
        Base( Kind::Call, std::move( p ), common::Operator::FunctionCall ),
        _name( std::move( name ) ),
        _params( std::move( params ) )
    {}

    const std::string &name() const {
        return _name;
    }

    const std::vector< EHandle > &parametres() const {
        return _params;
    }

private:
    std::string _name;
    std::vector< EHandle > _params;

};
} // namespace ast
} // namespace compiler
