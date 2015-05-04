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

