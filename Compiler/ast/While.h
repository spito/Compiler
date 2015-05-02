#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct While : Statement< Traversal > {

    using Base = Statement< Traversal >;

    While( common::Position p, EPtr condition, Ptr body ) :
        Base( p ),
        _condition( condition ),
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
