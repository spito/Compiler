#pragma once

#include "Statement.h"
#include "MemoryHolder.h"

namespace compiler {
namespace ast {

struct While : Statement, MemoryHolder {

    using Base = Statement;

    While( common::Position p ) :
        Base( Kind::While, std::move( p ) )
    {}

    EPtr condition() const {
        return _condition.get();
    }
    Ptr body() const {
        return _body.get();
    }

    void assign( EPtr c, Ptr b ) {
        _condition.reset( c );
        _body.reset( b );
    }

private:
    EHandle _condition;
    Handle _body;
};

} // namespace ast
} // namespace compiler
