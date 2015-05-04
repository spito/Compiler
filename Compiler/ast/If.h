#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

struct If : Statement {

    using Base = Statement;

    If ( common::Position p, EPtr condition, Ptr ifPath, Ptr elsePath ) :
        Base( Kind::If, std::move( p ) ),
        _condition( condition ),
        _ifPath( ifPath ),
        _elsePath( elsePath )
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

    void parentBreak( Ptr s ) override {
        _ifPath->parentBreak( s );
        _elsePath->parentBreak( s );
    }

    void parentContinue( Ptr s ) override {
        _ifPath->parentContinue( s );
        _elsePath->parentContinue( s );
    }

private:
    EHandle _condition;
    Handle _ifPath;
    Handle _elsePath;
};

} // namespace ast
} // namespace compiler
