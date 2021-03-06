#pragma once

#include "Statement.h"
#include "MemoryHolder.h"

#include <vector>

namespace compiler {
namespace ast {

struct Block : Statement, MemoryHolder {

    using MemoryHolder::add;

    Block( Block &&o ) :
        Statement( o ),
        MemoryHolder( o ),
        _descendants( std::move( o._descendants ) ),
        _soft( o._soft )
    {}

    Block( bool soft = true ) :
        Statement( Kind::Block ),
        _soft( soft )
    {}

    void import( Block &other ) {
        using std::swap;

        Statement::import( other );
        MemoryHolder::import( other );

        swap( _descendants, other._descendants );
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

    bool soft() const {
        return _soft;
    }

private:
    std::vector< Handle > _descendants;
    bool _soft;
};

} // namespace ast
} // namespace compiler
