#pragma once

#include "TypeStorage.h"

// expressions
#include "UnaryOperator.h"
#include "BinaryOperator.h"
#include "TernaryOperator.h"
#include "Call.h"
#include "Constant.h"
#include "Variable.h"

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

template< typename Traversal >
struct AST {

    void add( std::string name, Function< Traversal > f ) {
        if ( _functions.count( name ) )
            throw std::runtime_error( "multiple definition of function" );
        _functions.emplace( std::move( name ), std::move( f ) );
    }

    Block< Traversal > &global() {
        return _global;
    }

    const Function &findFunction( const std::string &name ) const {
        if ( !_functions.count( name ) )
            throw exception::InternalError( "function not found" );

        return _functions.find( name )->second;
    }

private:
    TypeStorage _typeStorage;
    std::map< std::string, Function< Traversal > > _functions;
    Block< Traversal > _global;
};

} // namespace ast
} // namespace compiler
