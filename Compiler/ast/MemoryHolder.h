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
        _memoryLength( 0 )
    {}

    void add( std::string name, const type::Type *type ) {
        if ( _variables.count( name ) )
            throw std::runtime_error( "duplicit symbol" );
        _variables.emplace( std::move( name ), Variable( _memoryLength, type ) );
        _memoryLength += type->size();
    }

    int memoryLength() const {
        return _memoryLength;
    }

    template< typename Yield >
    void forVariables( Yield yield ) const {
        for ( const auto &i : _variables )
            yield( i.first, i.second );
    }

    size_t size() const {
        return _variables.size();
    }

private:
    std::map< std::string, Variable > _variables;
    int _memoryLength;
};


} // namespace ast
} // namespace compiler
