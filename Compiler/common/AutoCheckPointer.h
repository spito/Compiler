#pragma once

#include "../includes/exceptions.h"

namespace compiler {
namespace common {

template< typename T >
struct AutoCheckPointer {

    AutoCheckPointer() :
        _value( nullptr )
    {}

    AutoCheckPointer( T *p ) :
        _value( p )
    {}

    AutoCheckPointer &operator=( T *p ) {
        _value = p;
        return *this;
    }

    T &operator*( ) const {
        if ( !_value )
            throw exception::InternalError( "Invalid dereference" );
        return *_value;
    }

    T *operator->( ) const {
        if ( !_value )
            throw exception::InternalError( "Invalid dereference" );
        return _value;
    }

    T *get() const {
        if ( !_value )
            throw exception::InternalError( "Invalid dereference" );
        return _value;
    }

    explicit operator bool() const {
        return _value != nullptr;
    }

private:
    T *_value;
};

} // namespace common
} // namespace compiler