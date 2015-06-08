#pragma once

#include "Type.h"

#include <string>
#include <map>

namespace compiler {
namespace ast {

struct MemoryHolder {

    struct Variable {

        Variable( int o, TypeOf t ) :
            _offset( o ),
            _type( std::move( t ) )
        {}

        int offset() const {
            return _offset;
        }
        int size() const {
            return _type.bytes();
        }
        const TypeOf &type() const {
            return _type;
        }

    private:
        int _offset;
        TypeOf _type;
    };

    MemoryHolder() :
        _memoryLength( 0 ),
        _variadic( false )
    {}

    MemoryHolder( const MemoryHolder & ) = default;

    MemoryHolder( MemoryHolder &&other ) :
        _memoryLength( other._memoryLength ),
        _variables( std::move( other._variables ) ),
        _ordering( std::move( other._ordering ) ),
        _prototypes( std::move( other._prototypes ) ),
        _variadic( other._variadic )
    {}

    bool add( std::string name, TypeOf type ) {
        if ( hasVariable( name ) )
            return false;
        _ordering.push_back( name );
        int bytes = type.bytes();
        _variables.emplace( std::move( name ), Variable( _memoryLength, std::move( type ) ) );
        _memoryLength += bytes;
        return true;
    }

    void addPrototype( TypeOf type ) {
        _prototypes.push_back( std::move ( type ) );
    }

    bool namePrototypes( std::vector< TypeOf > types, std::vector< std::string > names ) {
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

    template< typename Yield >
    void forOrderedVariables( Yield yield ) const {
        for ( const auto &name : _ordering ) {
            auto i = _variables.find( name );
            yield( i->first, i->second );
        }
    }

    template< typename Yield >
    void forPrototypes( Yield yield ) const {
        if ( _prototypes.empty() ) {
            for ( const auto &name : _ordering ) {
                auto i = _variables.find( name );
                yield( i->second.type() );
            }
        }
        else {
            for ( const ast::TypeOf &t : _prototypes ) {
                yield( t );
            }
        }
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
        swap( _ordering, other._ordering );
        swap( _prototypes, other._prototypes );
        swap( _memoryLength, other._memoryLength );
        swap( _variadic, other._variadic );
    }

    bool declarationOf( const MemoryHolder &other ) const {
        bool result = true;
        auto i = _prototypes.begin();
        other.forOrderedVariables( [&]( const std::string &, const Variable &v ) {
            result = result && v.type() == *i;
            ++i;
        } );
        return result;
    }
private:
    std::map< std::string, Variable > _variables;
    std::vector< std::string > _ordering;
    std::vector< TypeOf > _prototypes;
    int _memoryLength;
    bool _variadic;
};


} // namespace ast
} // namespace compiler
