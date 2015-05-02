#pragma once

#include "Block.h"
#include "Type.h"

namespace compiler {
namespace ast {

template< typename Traversal >
struct Function {

    Function( Function &&other ) :
        _parameters( std::move( other._parameters ) ),
        _body( std::move( other._body ) ),
        _returnType( std::move( other._returnType ) )
    {}

    Function( type::Type *t ) :
        _returnType( *t )
    {}

    MemoryHolder &parameters() {
        return _parameters;
    }
    const MemoryHolder &parameters() const {
        return _parameters;
    }

    Block< Traversal > &body() {
        return _body;
    }
    const Block< Traversal > &body() const {
        return _body;
    }

    const type::Type &returnType() const {
        return _returnType;
    }

private:
    MemoryHolder _parameters;
    Block< Traversal > _body;
    const type::Type &_returnType;
};


} // namespace ast
} // namespace compiler
