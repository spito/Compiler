#pragma once

#include "Block.h"
#include "Type.h"

#include "../includes/exceptions.h"

namespace compiler {
namespace ast {

struct Function {

    Function( Function &&other ) :
        _parameters( std::move( other._parameters ) ),
        _body( std::move( other._body ) ),
        _returnType( std::move( other._returnType ) ),
        _definition( other._definition )
    {}

    Function( const type::Type *t, std::string name, bool definition ) :
        _body( false ),
        _returnType( *t ),
        _name( std::move( name ) ),
        _definition( definition )
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

    bool definition() const {
        return _definition;
    }

    void import( Function &f ) {
        using std::swap;

        if ( !f.definition() )
            return;

        if ( name() != f.name() )
            throw exception::InternalError( "mismatch between function names" );

        if ( returnType() != f.returnType() )
            throw exception::InternalError( "mismatch between function return types" );

        if ( !parameters().declarationOf( f.parameters() ) )
            throw exception::InternalError( "mismatch between parameters inside function" );

        _parameters.import( f._parameters );
        _body.import( f._body );
        _definition = f.definition();
    }

private:
    MemoryHolder _parameters;
    Block _body;
    const type::Type &_returnType;
    std::string _name;
    bool _definition;
};


} // namespace ast
} // namespace compiler
