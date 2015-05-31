#pragma once

#include "Core.h"
#include "../common/Utils.h"
#include "../includes/exceptions.h"

#include <cstdint>
#include <memory>

namespace compiler {
namespace ast {

struct TypeOf : common::Comparable {

    enum class Kind {
        Elementary,
        Array,
        Pointer,
    };

    TypeOf() :
        _kind( Kind::Elementary ),
        _bytes( 0 ),
        _signed( false ),
        _const( false ),
        _count( 0 )
    {}

    TypeOf( int bytes, bool sign, bool constness = false ) :
        _kind( Kind::Elementary ),
        _bytes( bytes ),
        _signed( sign ),
        _const( constness ),
        _count( 1 )
    {}

    TypeOf( const TypeOf &other ) :
        _kind( other._kind ),
        _bytes( other._bytes ),
        _signed( other._signed ),
        _const( other._const ),
        _count( other._count ),
        _of( other._of ? new TypeOf( *other._of.get() ) : nullptr )
    {}

    TypeOf( TypeOf &&other ) :
        _kind( other._kind ),
        _bytes( other._bytes ),
        _signed( other._signed ),
        _const( other._const ),
        _count( other._count ),
        _of( std::move( other._of ) )
    {}

    TypeOf &operator=( TypeOf other ) {
        swap( other );
        return *this;
    }

    void swap( TypeOf &other ) {
        using std::swap;

        swap( _kind, other._kind );
        swap( _bytes, other._bytes );
        swap( _signed, other._signed );
        swap( _const, other._const );
        swap( _count, other._count );
        swap( _of, other._of );
    }

    Kind kind() const {
        return _kind;
    }

    int bytes() const {
        return _bytes;
    }
    bool isSigned() const {
        return _signed;
    }
    bool isConst() const {
        return _const;
    }
    int count() const {
        return _count;
    }
    const TypeOf *of() const {
        return _of.get();
    }

    static TypeOf makeArray( int count, TypeOf *of, bool constness = false ) {
        if ( !of )
            throw exception::InternalError( "not base type" );
        return TypeOf( Kind::Array, count, count * of->bytes(), of, constness );
    }

    static TypeOf makePointer( TypeOf *of, bool constness = false ) {
        if ( !of )
            throw exception::InternalError( "not base type" );
        return TypeOf( Kind::Pointer, 1, sizeof( void * ), of, constness );
    }

    static TypeOf makeConst( TypeOf t, bool constness = true ) {
        t._const = constness;
        return t;
    }

    bool operator==( const TypeOf &other ) const {
        bool partial =
            _kind == other._kind &&
            _bytes == other._bytes &&
            _signed == other._signed &&
            _count == other._count;

        if ( !partial )
            return false;

        if ( _of == other._of )
            return true;

        if ( !_of || !other._of )
            return false;
             
        return *_of == *other._of;
    }

private:
    TypeOf( Kind kind, int count, int bits, TypeOf *of, bool constness) :
        _kind( kind ),
        _bytes( bits ),
        _signed( false ),
        _const( constness ),
        _count( count ),
        _of( of )
    {}

    Kind _kind;
    int _bytes;
    bool _signed;
    bool _const;
    int _count;
    std::unique_ptr< TypeOf > _of;
};

struct ProxyType {

    ProxyType( TypeOf t ) :
        _type( std::move( t ) )
    {}

    ProxyType array( int count, bool constness = false ) const {
        return ProxyType( TypeOf::makeArray( count, new TypeOf( _type ), constness ) );
    }

    ProxyType pointer( bool constness = false ) const {
        return ProxyType( TypeOf::makePointer( new TypeOf( _type ), constness ) );
    }

    ProxyType constness( bool c = true ) const {
        return ProxyType( TypeOf::makeConst( _type, c ) );
    }

    operator TypeOf() const {
        return _type;
    }

    const TypeOf &get() const {
        return _type;
    }

private:
    TypeOf _type;
};


} // namespace ast
} // namespace compiler
