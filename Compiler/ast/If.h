#pragma once

#include "Statement.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct If : Statement< Traversal > {

    using Base = Statement< Traversal >;

    If ( common::Position p, EPtr condition, Ptr ifPath, Ptr elsePath ) :
        Base( p ),
        _condition( condition ),
        _ifPath( ifPath ),
        _elsePath( elsePath )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

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
