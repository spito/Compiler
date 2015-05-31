#pragma once

#include "Type.h"
#include "../includes/exceptions.h"

#include <map>
#include <string>

namespace compiler {
namespace ast {

struct TypeStorage {

    static ProxyType type( const std::string &name ) {
        auto i = _namedTypes.find( name );
        if ( i == _namedTypes.end() )
            throw exception::InternalError( "name not found" );
        return ProxyType( i->second );
    }


private:
    static std::map< std::string, TypeOf > _namedTypes;

};

} // namespace ast
} // namespace compiler
