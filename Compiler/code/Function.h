#pragma once

#include "BasicBlock.h"

#include <string>
#include <vector>

namespace compiler {
namespace code {

struct Function {
    using Iterator = std::vector< BasicBlock >::const_iterator;

    Function( Type returnType,
              std::string name,
              std::vector< Register > namedRegisters,
              std::vector< BasicBlock > basicBlocks ) :
        _type( returnType ),
        _name( std::move( name ) ),
        _namedRegisters( std::move( namedRegisters ) ),
        _basicBlocks( std::move( basicBlocks ) )
    {}

    Function( const Function & ) = default;
    //Function( Function && ) = default;
    Function( Function &&other ) :
        _type( std::move( other._type ) ),
        _name( std::move( other._name ) ),
        _namedRegisters( std::move( other._namedRegisters ) ),
        _basicBlocks( std::move( other._basicBlocks ) )
    {}

    const std::string &name() const {
        return _name;
    }

    const Type &returnType() const {
        return _type;
    }

    const std::vector< BasicBlock > &basicBlocks() const {
        return _basicBlocks;
    }

    const std::vector< Register > &namedRegisters() const {
        return _namedRegisters;
    }


    Iterator begin() const {
        return _basicBlocks.begin();
    }

    Iterator end() const {
        return _basicBlocks.end();
    }


private:
    Type _type;
    std::string _name;
    std::vector< Register > _namedRegisters;
    std::vector< BasicBlock > _basicBlocks;
};

} // namespace code
} // namespace compiler
