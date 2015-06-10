#pragma once

#include "../common/Utils.h"

#include <vector>

namespace compiler {
namespace code {

struct Type : common::Comparable {

    Type( int bits ) :
        _bits( bits )
    {}

    Type( int bits, int indirection ) :
        _bits( bits )
    {
        for ( int i = 0; i < indirection; ++i )
            addIndirection();
    }

    Type( const Type & ) = default;
    Type( Type &&other ) :
        _bits( other._bits ),
        _decoration( std::move( other._decoration ) )
    {}

    Type &operator=( Type other ) {
        swap( other );
        return *this;
    }

    void swap( Type &other ) {
        using std::swap;

        swap( _bits, other._bits );
        swap( _decoration, other._decoration );
    }

    int bits() const {
        return _bits;
    }

    bool isElementary() const {
        return _decoration.empty();
    }

    bool isArray() const {
        return
            !isElementary() &&
            _decoration.back() != Pointer;
    }

    bool isPointer() const {
        return
            !isElementary() &&
            _decoration.back() == Pointer;
    }

    const std::vector< int > &decoration() const {
        return _decoration;
    }

    void addDimension( int d ) {
        _decoration.push_back( d );
    }
    void removeDimension() {
        if ( isArray() )
            _decoration.pop_back();
    }

    void addIndirection() {
        _decoration.push_back( Pointer );
    }
    void removeIndirection() {
        if ( isPointer() )
            _decoration.pop_back();
    }

    enum {
        Pointer = -1
    };

    bool operator==( const Type &other ) const {
        return
            _bits == other._bits &&
            _decoration == other._decoration;
    }

private:
    int _bits;
    std::vector< int > _decoration;
};

} // namespace code
} // namespace compiler

namespace std {

template<>
inline void swap< compiler::code::Type >( compiler::code::Type &lhs, compiler::code::Type &rhs ) {
    lhs.swap( rhs );
}

} // namespace std
