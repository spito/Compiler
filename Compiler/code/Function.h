#pragma once

#include "BasicBlock.h"

#include <string>
#include <vector>
#include <map>

namespace compiler {
namespace code {

struct Function {

    Function( Type returnType,
              std::string name,
              std::vector< Register > arguments,
              std::vector< Register > namedRegisters,
              std::map< int, BasicBlock > basicBlocks ) :
        _type( returnType ),
        _name( std::move( name ) ),
        _arguments( std::move( arguments ) ),
        _namedRegisters( std::move( namedRegisters ) ),
        _basicBlocks( std::move( basicBlocks ) )
    {}

    Function( const Function & ) = default;
    //Function( Function && ) = default;
    Function( Function &&other ) :
        _type( std::move( other._type ) ),
        _name( std::move( other._name ) ),
        _arguments( std::move( other._arguments ) ),
        _namedRegisters( std::move( other._namedRegisters ) ),
        _basicBlocks( std::move( other._basicBlocks ) )
    {}

    const std::string &name() const {
        return _name;
    }

    const Type &returnType() const {
        return _type;
    }

    const std::map< int, BasicBlock > &basicBlocks() const {
        return _basicBlocks;
    }

    const std::vector< Register > &namedRegisters() const {
        return _namedRegisters;
    }

    const std::vector< Register > &arguments() const {
        return _arguments;
    }

private:
    Type _type;
    std::string _name;
    std::vector< Register > _arguments;
    std::vector< Register > _namedRegisters;
    std::map< int, BasicBlock > _basicBlocks;
};

} // namespace code
} // namespace compiler
