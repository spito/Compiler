#pragma once

#include "Statement.h"
#include "MemoryHolder.h"

namespace compiler {
namespace ast {

struct If : Statement, MemoryHolder {

    using Base = Statement;

    If ( common::Position p ) :
        Base( Kind::If, std::move( p ) )
    {}

    EPtr condition() const {
        return _condition.get();
    }
    Ptr ifPath() const {
        return _ifPath.get();
    }
    Ptr elsePath() const {
        return _elsePath.get();
    }

    void assign( EPtr c, Ptr i, Ptr e ) {
        _condition.reset( c );
        _ifPath.reset( i );
        _elsePath.reset( e );
    }

private:
    EHandle _condition;
    Handle _ifPath;
    Handle _elsePath;
};

} // namespace ast
} // namespace compiler
