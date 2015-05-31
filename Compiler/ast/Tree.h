#pragma once

#include "TypeStorage.h"

// expressions
#include "Constant.h"
#include "StringPlaceholder.h"
#include "ArrayInitiaizer.h"
#include "Variable.h"
#include "UnaryOperator.h"
#include "BinaryOperator.h"
#include "TernaryOperator.h"
#include "Call.h"

// statements
#include "Block.h"
#include "If.h"
#include "While.h"
#include "DoWhile.h"
#include "For.h"
// #include "Switch"

#include "Break.h"
#include "Continue.h"
#include "Return.h"
// #include "Case.h"
// #include "Default.h"
// #include "Goto.h"

#include "Function.h"

#include "../includes/exceptions.h"

namespace compiler {
namespace ast {

struct Tree {

    using FunctionHandle = std::unique_ptr < Function > ;

    Tree() = default;

    Tree( const Tree & ) = delete;

    Tree( Tree &&o ) :
        _functions( std::move( o._functions ) ),
        _global( std::move( o._global ) )
    {}

    void add( FunctionHandle &&f ) {
        std::string name = f->name();
        auto i = _functions.find( name );
        if ( i != _functions.end() ) {
            if ( i->second->definition() && f->definition() )
                throw exception::InternalError( "multiple definition of function" );
            i->second->import( *f );
        }
        else 
            _functions.emplace( std::move( name ), std::move( f ) );
    }

    template< typename S, typename... Args >
    S *make( Args &&... args ) {
        return new S( common::Position(), std::forward< Args >( args )... );
    }

    Block &global() {
        return _global;
    }

    const Block &global() const {
        return _global;
    }

    const Function &findFunction( const std::string &name ) const {
        if ( !_functions.count( name ) )
            throw exception::InternalError( "function not found" );

        return *_functions.find( name )->second.get();
    }
    Function &findFunction( const std::string &name ) {
        if ( !_functions.count( name ) )
            throw exception::InternalError( "function not found" );

        return *_functions.find( name )->second.get();
    }

    template< typename Yield >
    void forFunctions( Yield yield ) const {
        for ( const auto &i : _functions ) {
            yield( i.second.get() );
        }
    }

private:
    std::map< std::string, FunctionHandle > _functions;
    Block _global;
};

} // namespace ast
} // namespace compiler
