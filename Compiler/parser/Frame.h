#pragma once

#include "../ast/Tree.h"

#include <map>

namespace compiler {
namespace parser {

struct Frame {

    Frame( const ast::MemoryHolder *block )
    {
        block->forVariables( [this]( std::string name, ast::MemoryHolder::Variable v ) {
            _types.emplace( std::move( name ), v.type() );
        } );
    }
    Frame( const ast::MemoryHolder &block ) :
        Frame( &block )
    {}

    const ast::TypeOf *variable( const std::string &name ) const {
        auto i = _types.find( name );
        if ( i == _types.end() )
            return nullptr;
        return &i->second;
    }

private:
    std::map< std::string, ast::TypeOf > _types;
};


} // namespace parser
} // namespace compiler
