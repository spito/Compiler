#pragma once

#include "Type.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace compiler {
namespace ast {

struct TypeStorage {
    using Handle = std::unique_ptr< type::Type >;
    using Ptr = type::Type *;
    using ConstPtr = const type::Type *;

    template< typename T, typename... Args >
    Ptr addType( std::string name, Args &&... args ) ->
        std::enable_if< std::is_base_of< Ptr, T >::value, Ptr >
    {
        return addType( std::move( name ), new T( std::forward< Args >( args )... ) );
    }

    template< typename T, typename... Args >
    auto addType( Args &&... args ) ->
        std::enable_if< std::is_base_of< Ptr, T >::value, Ptr >
    {
        return addType( new T( std::forward< Args >( args )... ) );
    }

private:
    Ptr addType( std::string name, Ptr t ) {
        return _named.insert( { std::move( name ), Handle( t ) } ).first->second.get();
    }

    Ptr addType( Ptr t ) {
        return _unnamed.insert( Handle( t ) ).first->get();
    }

    std::unordered_map< std::string, Handle > _named;
    std::unordered_set< Handle > _unnamed;
};

} // namespace ast
} // namespace compiler

namespace std {

template<>
struct hash< std::unique_ptr< compiler::ast::type::Type > > {

    using Handle = std::unique_ptr< compiler::ast::type::Type >;

    size_t operator()( const Handle &h ) const {
        return h->hash();
    }
};

} // namespace std
