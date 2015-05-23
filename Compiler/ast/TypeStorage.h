#pragma once

#include "Type.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace std {

template<>
struct hash< std::unique_ptr< compiler::ast::type::Type > > {

    using Handle = std::unique_ptr< compiler::ast::type::Type >;

    size_t operator()( const Handle &h ) const {
        return h->hash();
    }
};

} // namespace std

namespace compiler {
namespace ast {

struct TypeStorage {
    using Handle = std::unique_ptr< type::Type >;
    using Ptr = type::Type *;
    using ConstPtr = const type::Type *;

    TypeStorage() = default;
    TypeStorage( const TypeStorage & ) = delete;
    TypeStorage( TypeStorage &&o ) :
        _named( std::move( o._named ) ),
        _unnamed( std::move( o._unnamed ) )
    {}

    template< typename T, typename... Args >
    auto addType( const char *name, Args &&... args ) ->
        typename std::enable_if< std::is_base_of< type::Type, T >::value, ConstPtr >::type
    {
        return insertNamed( name, new T( std::forward< Args >( args )... ) );
    }

    template< typename T, typename... Args >
    auto addType( Args &&... args ) ->
        typename std::enable_if< std::is_base_of< type::Type, T >::value, ConstPtr >::type
    {
        return insertUnnamed( new T( std::forward< Args >( args )... ) );
    }

    ConstPtr fetchType( const std::string &name ) const {
        auto i = _named.find( name );
        if ( i == _named.end() )
            return nullptr;
        return i->second.get();
    }

private:
    ConstPtr insertNamed( std::string name, Ptr t ) {
        return _named.emplace( std::move( name ), Handle( t ) ).first->second.get();
    }

    ConstPtr insertUnnamed( Ptr t ) {
        return _unnamed.insert( Handle( t ) ).first->get();
    }

    std::unordered_map< std::string, Handle > _named;
    std::unordered_set< Handle > _unnamed;
};

} // namespace ast
} // namespace compiler
