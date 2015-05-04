#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct DoWhile : Statement {

    using Base = Statement;

    DoWhile( common::Position p, EPtr condition, Ptr body ) :
        Base( Kind::DoWhile, std::move( p ) ),
        _condition( condition ),
        _body( body )
    {
        parentBreak( this );
        parentContinue( this );
        _body->parentBreak( this );
        _body->parentContinue( this );
    }

    EPtr condition() const {
        return _condition.get();
    }
    Ptr body() const {
        return _body.get();
    }

private:
    EHandle _condition;
    Handle _body;
};

} // namespace ast
} // namespace compiler
