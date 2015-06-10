#pragma once

#include "Function.h"
#include "Prototype.h"

#include <map>
#include <algorithm>

namespace compiler {
namespace code {

struct Code {

    void addGlobals( std::vector< Instruction > &globals ) {
        _globals.swap( globals );
    }

    void addFunction( Type type,
                      const std::string &name,
                      std::vector< Register > arguments,
                      std::vector< Register > namedRegisters,
                      std::map< int, BasicBlock > instructions ) {
        _functions.emplace( name, Function( std::move( type ), name, std::move( arguments ), std::move( namedRegisters ), std::move( instructions ) ) );
    }

    void addDeclaration( Type type,
                         std::string name,
                         std::vector< Type > argumentTypes ) {
        _declarations.emplace_back( std::move( type ), std::move( name ), std::move( argumentTypes ) );
    }

    const Function *function( const std::string &name ) {
        auto i = _functions.find( name );
        if ( i == _functions.end() )
            return nullptr;
        return &i->second;
    }

    const std::map< std::string, Function > &functions() const {
        return _functions;
    }

    const std::vector< Prototype > &declarations() const {
        return _declarations;
    }

    const std::vector< Instruction > &globals() const {
        return _globals;
    }

    const Prototype *declaration( const std::string &name ) const {
        auto i = std::find_if( _declarations.begin(), _declarations.end(), [&]( const Prototype &p ) {
            return p.name() == name;
        } );
        if ( i == _declarations.end() )
            return nullptr;
        return &*i;
    }

private:
    std::vector< Instruction > _globals;
    std::vector< Prototype > _declarations;
    std::map< std::string, Function > _functions;
};

} // namespace code
} // namespace compiler

