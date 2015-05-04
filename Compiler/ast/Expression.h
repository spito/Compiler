#pragma once

#include "Statement.h"
#include "../common/Operator.h"

namespace compiler {
namespace ast {

struct Expression : Statement {
    using Base = Statement;


    Expression( Kind k, common::Position p, common::Operator op ) :
        Base( k, std::move( p ) ),
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
