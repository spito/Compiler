#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct DoWhile : Cycle {

    DoWhile( common::Position p ) :
        Base( Kind::DoWhile, std::move( p ) )
    {}

    EPtr condition() const {
        return _condition.get();
    }
    Ptr body() const {
        return _body.get();
    }

    void assign( EPtr condition, Ptr body ) {
        _condition.reset( condition );
        _body.reset( body );
    }

private:
    EHandle _condition;
    Handle _body;
};

} // namespace ast
} // namespace compiler
