#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct For : Statement< Traversal > {

    using Base = Statement< Traversal >;

    For( common::Position p, EPtr initialization, EPtr condition, EPtr increment, Ptr body ) :
        Base( p ),
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

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
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
