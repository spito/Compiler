#pragma once

#include "Type.h"

#include <string>
#include <vector>

namespace compiler {
namespace code {

struct Prototype {

    Prototype( Type returnType, std::string name, std::vector< Type > argumentTypes ) :
        _name( std::move( name ) ),
        _returnType( std::move( returnType ) ),
        _argumentTypes( std::move( argumentTypes ) )
    {}

    const std::string &name() const {
        return _name;
    }

    const Type &returnType() const {
        return _returnType;
    }

    const std::vector< Type > &argumentTypes() const {
        return _argumentTypes;
    }

private:
    std::string _name;
    Type _returnType;
    std::vector< Type > _argumentTypes;
};

} // namespace code
} // namespace compiler
