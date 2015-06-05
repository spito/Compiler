#pragma once

#include "Type.h"

#include <string>

namespace compiler {
namespace code {


struct Register {
    Register( std::string name, Type type ) :
        _name( std::move( name ) ),
        _type( type )
    {}

    const std::string &name() const {
        return _name;
    }

    Type type() const {
        return _type;
    }
private:
    std::string _name;
    Type _type;
};

} // namespace code
} // namespace compiler
