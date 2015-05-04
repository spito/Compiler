#pragma once

#include "Block.h"
#include "Type.h"

namespace compiler {
namespace ast {

struct Function {

    Function( Function &&other ) :
        _parameters( std::move( other._parameters ) ),
        _body( std::move( other._body ) ),
        _returnType( std::move( other._returnType ) )
    {}

    Function( const type::Type *t, std::string name ) :
        _returnType( *t ),
        _name( std::move( name ) )
    {}

    MemoryHolder &parameters() {
        return _parameters;
    }
    const MemoryHolder &parameters() const {
        return _parameters;
    }

    Block &body() {
        return _body;
    }
    const Block &body() const {
        return _body;
    }

    const type::Type &returnType() const {
        return _returnType;
    }
    const std::string &name() const {
        return _name;
    }

private:
    MemoryHolder _parameters;
    Block _body;
    const type::Type &_returnType;
    std::string _name;
};


} // namespace ast
} // namespace compiler