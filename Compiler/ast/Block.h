#pragma once

#include "Statement.h"
#include "MemoryHolder.h"

#include <vector>

namespace compiler {
namespace ast {

template< typename Traversal >
struct Block : Statement< Traversal >, MemoryHolder {
    using Iterator = std::vector< Handle >::iterator;

    using MemoryHolder::add;

    Block( Block &&o ) :
        _descendants( std::move( o._descendants ) ),
        _soft( o._soft )
    {}

    Block( bool soft = true ) :
        _soft( soft )
    {}

    Information *traverse( Traversal &t ) const override {
        return t.eval( this );
    }

    void add( Ptr p ) {
        _descendants.emplace_back( p );
    }

    template< typename Yield >
    void forDescendatns( Yield yield ) const {
        for ( const auto &i : _descendants ) {
            if ( !yield( i.get() ) )
                break;
        }
    }

    void parentBreak( Ptr s ) override {
        for ( auto &i : _descendants )
            i->parentBreak( s );
    }
    void parentContinue( Ptr s ) override {
        for ( auto &i : _descendants )
            i->parentContinue( s );
    }

    bool soft() const {
        return _soft;
    }

private:
    std::vector< Handle > _descendants;
    bool _soft;
};

} // namespace ast
} // namespace compiler
