#pragma once

#include "../ast/AST.h"

namespace compiler {
namespace interpret {

struct Variable {

    Variable( char *data, ast::MemoryHolder::Variable v ) :
        _address( data + v.offset() ),
        _type( &v.type() )
    {}

    Variable( void *address, const ast::type::Type *type ) :
        _address( address ),
        _type( type )
    {}

    Variable() :
        _address( nullptr ),
        _type( nullptr )
    {}

    void *address() const {
        return _address;
    }
    const ast::type::Type *type() const {
        return _type;
    }
    bool valid() const {
        return _address != nullptr;
    }

    Variable operator[]( uint64_t index ) const {
        const ast::type::Type *of = nullptr;
        char *base = nullptr;

        switch ( _type->kind() ) {
        case ast::type::Kind::Array:
            of = &_type->as< ast::type::Array >()->of();
            base = &as< char >();
            break;
        case ast::type::Kind::Pointer:
            of = &_type->as< ast::type::Pointer >()->of();
            base = as< char * >();
            break;
        case ast::type::Kind::Elementary:
            throw exception::InternalError( "cannot use array access operator to elementary" );
        }

        return Variable( base + index * of->size(), of );
    }

    template< typename Yield >
    void flatten( Yield yield ) {
        if ( _type->kind() != ast::type::Kind::Array )
            return;
        const ast::type::Array *type = _type->as< ast::type::Array >();

        const ast::type::Type *base = _type;
        while ( base->kind() == ast::type::Kind::Array ) {
            base = &base->as< ast::type::Array >()->of();
        }

        int count = _type->size() / base->size();
        char *storage = &as< char >();
        for ( int i = 0; i < count; ++i ) {
            yield( Variable( storage, base ) );
            storage += base->size();
        }
    }

    template< typename T >
    T &as() const {
        return *static_cast<T *>( _address );
    }

private:

    void *_address;
    const ast::type::Type *_type;
};

} // namespace interpret
} // namespace compiler

