#pragma once

#include "../common/Token.h"
#include "../includes/exceptions.h"

#include <memory>
#include <vector>

namespace compiler {
namespace preprocessor {

struct IteratorPack {
    using Iterator = std::vector< common::Token >::iterator;

    IteratorPack( Iterator b, Iterator e ) :
        begin( b ),
        end( e )
    {}

    Iterator begin;
    Iterator end;

    bool valid() const {
        return begin != end;
    }
    explicit operator bool() const {
        return valid();
    }

    IteratorPack &operator++( ) {
        ++begin;
        return *this;
    }

    const common::Token *operator->( ) const {
        return begin.operator->();
    }
    common::Token *operator->( ) {
        return begin.operator->( );
    }
    const common::Token &operator*( ) const {
        return *begin;
    }
    common::Token &operator*( ) {
        return *begin;
    }

};

bool expression( IteratorPack && );

} // namespace preprocessor
} // namespace compiler

