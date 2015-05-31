#pragma once

#include "../ast/Tree.h"

namespace compiler {
namespace interpret {

struct Variable {

    Variable( char *data, ast::MemoryHolder::Variable v ) :
        _address( data + v.offset() ),
        _type( v.type() )
    {}

    Variable( void *address, ast::TypeOf type ) :
        _address( address ),
        _type( type )
    {}

    Variable() :
        _address( nullptr ),
        _type()
    {}

    void *address() const {
        return _address;
    }
    const ast::TypeOf &type() const {
        return _type;
    }
    bool valid() const {
        return _address != nullptr;
    }

    Variable operator[]( uint64_t index ) const {
        char *base = nullptr;

        if ( !_type.of() )
            throw exception::InternalError( "cannot use array access operator to elementary" );

        switch ( _type.kind() ) {
        case ast::TypeOf::Kind::Array:
            base = &as< char >();
            break;
        case ast::TypeOf::Kind::Pointer:
            base = as< char * >();
            break;
        case ast::TypeOf::Kind::Elementary:
            throw exception::InternalError( "cannot use array access operator to elementary" );
        }

        return Variable( base + index * _type.of()->bytes(), *_type.of() );
    }

    template< typename Yield >
    void flatten( Yield yield ) {
        if ( _type.kind() != ast::TypeOf::Kind::Array )
            return;

        const ast::TypeOf *base = &_type;
        while ( base->kind() == ast::TypeOf::Kind::Array )
            base = base->of();

        int count = _type.bytes() / base->bytes();
        char *storage = &as< char >();
        for ( int i = 0; i < count; ++i ) {
            yield( Variable( storage, *base ) );
            storage += base->bytes();
        }
    }

    template< typename T >
    T &as() const {
        return *static_cast<T *>( _address );
    }

private:

    void *_address;
    ast::TypeOf _type;
};

} // namespace interpret
} // namespace compiler

