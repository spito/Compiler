#pragma once

#include "Statement.h"
#include "MemoryHolder.h"

namespace compiler {
namespace ast {

struct For : Cycle, MemoryHolder {

    For( common::Position p ) :
        Base( Kind::For, std::move( p ) )
    {}

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

    void assign( EPtr ini, EPtr c, EPtr inc, Ptr b ) {
        _initialization.reset( ini );
        _condition.reset( c );
        _increment.reset( inc );
        _body.reset( b );
    }

private:
    EHandle _initialization;
    EHandle _condition;
    EHandle _increment;
    Handle _body;
};

} // namespace ast
} // namespace compiler
