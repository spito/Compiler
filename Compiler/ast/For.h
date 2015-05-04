#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct For : Statement {

    using Base = Statement;

    For( common::Position p, EPtr initialization, EPtr condition, EPtr increment, Ptr body ) :
        Base( Kind::For, std::move( p ) ),
        _initialization( initialization ),
        _condition( condition ),
        _increment( increment ),
        _body( body )
    {
        parentBreak( this );
        parentContinue( this );
        _body->parentBreak( this );
        _body->parentContinue( this );
    }

    EPtr initialization() const {
        return _initialization.get();
    }
    EPtr condition() const {
        return _condition.get();
    }
    EPtr increment() const {
        return _increment.get();
    }
    Ptr body() const {
        return _body.get();
    }

private:
    EHandle _initialization;
    EHandle _condition;
    EHandle _increment;
    Handle _body;
};

} // namespace ast
} // namespace compiler
