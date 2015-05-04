#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct Return : Statement {
    using Base = Statement;

    Return( common::Position p, EPtr e ) :
        Base( Kind::Return, std::move( p ) ),
        _expression( e )
    {}

    EPtr expression() const {
        return _expression.get();
    }

private:
    EHandle _expression;
};

} // namespace ast
} // namespace compiler
