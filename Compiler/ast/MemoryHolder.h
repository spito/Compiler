#pragma once

#include "Type.h"

#include <string>
#include <map>

namespace compiler {
namespace ast {

struct MemoryHolder {

    struct Variable {

        Variable( int o, const type::Type *t ) :
            _offset( o ),
            _type( *t )
        {}

        int offset() const {
            return _offset;
        }
        int size() const {
            return _type.size();
        }
        const type::Type &type() const {
            return _type;
        }

    private:
        int _offset;
        const type::Type &_type;
    };

    MemoryHolder() :
        _memoryLength( 0 ),
        _variadic( false )
    {}

    MemoryHolder( const MemoryHolder & ) = default;

    MemoryHolder( MemoryHolder &&other ) :
        _memoryLength( other._memoryLength ),
        _variables( std::move( other._variables ) ),
        _prototypes( std::move( other._prototypes ) ),
        _variadic( other._variadic )
    {}

    bool add( std::string name, const type::Type *type ) {
        if ( hasVariable( name ) )
            return false;
        _variables.emplace( std::move( name ), Variable( _memoryLength, type ) );
        _memoryLength += type->size();
        return true;
    }

    void addPrototype( const type::Type *type ) {
        _prototypes.push_back( type );
    }

    bool namePrototypes( std::vector< const type::Type * > types, std::vector< std::string > names ) {
        if ( _prototypes != types )
            return false;

        for ( size_t i = 0; i < names.size(); ++i )
            add( std::move( names[ i ] ), types[ i ] );
        if ( names.size() + 1 == types.size() )
            _variadic = true;

        _prototypes.clear();
        return true;
    }

    int memoryLength() const {
        return _memoryLength;
    }

    bool variadic() const {
        return _variadic;
    }

    template< typename Yield >
    void forVariables( Yield yield ) const {
        for ( const auto &i : _variables )
            yield( i.first, i.second );
    }

    bool hasVariable( const std::string &name ) {
        return _variables.count( name ) == 1;
    }

    const Variable *getVariable( const std::string &name ) const {
        auto i = _variables.find( name );
        if ( i == _variables.end() )
            return nullptr;
        return &i->second;
    }

    size_t size() const {
        return _variables.size();
    }

    void import( MemoryHolder &other ) {
        using std::swap;

        swap( _variables, other._variables );
        swap( _prototypes, other._prototypes );
        swap( _memoryLength, other._memoryLength );
        swap( _variadic, other._variadic );
    }

    bool declarationOf( const MemoryHolder &other ) const {
        return _prototypes == other._prototypes;
    }
private:
    std::map< std::string, Variable > _variables;
    std::vector< const type::Type * > _prototypes;
    int _memoryLength;
    bool _variadic;
};


} // namespace ast
} // namespace compiler
