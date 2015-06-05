#pragma once

#include "Function.h"

#include <map>

namespace compiler {
namespace code {

struct Code {

    void addGlobals( std::vector< Instruction > &globals ) {
        _globals.swap( globals );
    }

    void addFunction( code::Type type,
                      const std::string &name,
                      std::vector< Register > namedRegisters,
                      std::vector< BasicBlock > instructions ) {
        _functions.emplace( name, Function( std::move( type ), name, std::move( namedRegisters ), std::move( instructions ) ) );
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

    const std::vector< Instruction > &globals() const {
        return _globals;
    }

private:
    std::vector< Instruction > _globals;
    std::map< std::string, Function > _functions;
};

} // namespace code
} // namespace compiler

