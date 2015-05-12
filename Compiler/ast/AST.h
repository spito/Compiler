#pragma once

#include "TypeStorage.h"

// expressions
#include "Constant.h"
#include "StringPlaceholder.h"
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
#include "Traversal.h"

#include "../includes/exceptions.h"

namespace compiler {
namespace ast {

struct AST {

    AST() {
        _typeStorage.addType< type::Elementary >( "void", 0, true );
        _typeStorage.addType< type::Elementary >( "char", 1, true );
        _typeStorage.addType< type::Elementary >( "signed char", 1, true );
        _typeStorage.addType< type::Elementary >( "unsigned char", 1, false );
        _typeStorage.addType< type::Elementary >( "short", 2, true );
        _typeStorage.addType< type::Elementary >( "short int", 2, true );
        _typeStorage.addType< type::Elementary >( "signed short", 2, true );
        _typeStorage.addType< type::Elementary >( "signed short int", 2, true );
        _typeStorage.addType< type::Elementary >( "unsigned short", 2, false );
        _typeStorage.addType< type::Elementary >( "unsigned short int", 2, false );
        _typeStorage.addType< type::Elementary >( "int", 4, true );
        _typeStorage.addType< type::Elementary >( "signed", 4, true );
        _typeStorage.addType< type::Elementary >( "signed int", 4, true );
        _typeStorage.addType< type::Elementary >( "unsigned int", 4, false );
        _typeStorage.addType< type::Elementary >( "unsigned", 4, false );
        _typeStorage.addType< type::Elementary >( "long", 8, true );
        _typeStorage.addType< type::Elementary >( "long int", 8, true );
        _typeStorage.addType< type::Elementary >( "signed long", 8, true );
        _typeStorage.addType< type::Elementary >( "signed long int", 8, true );
        _typeStorage.addType< type::Elementary >( "unsigned long", 8, false );
        _typeStorage.addType< type::Elementary >( "unsigned long int", 8, false );
        _typeStorage.addType< type::Elementary >( "const void", 0, true, true );
        _typeStorage.addType< type::Elementary >( "const char", 1, true, true );
        _typeStorage.addType< type::Elementary >( "const signed char", 1, true, true );
        _typeStorage.addType< type::Elementary >( "const unsigned char", 1, false, true );
        _typeStorage.addType< type::Elementary >( "const short", 2, true, true );
        _typeStorage.addType< type::Elementary >( "const short int", 2, true, true );
        _typeStorage.addType< type::Elementary >( "const signed short", 2, true, true );
        _typeStorage.addType< type::Elementary >( "const signed short int", 2, true, true );
        _typeStorage.addType< type::Elementary >( "const unsigned short", 2, false, true );
        _typeStorage.addType< type::Elementary >( "const unsigned short int", 2, false, true );
        _typeStorage.addType< type::Elementary >( "const int", 4, true, true );
        _typeStorage.addType< type::Elementary >( "const signed", 4, true, true );
        _typeStorage.addType< type::Elementary >( "const signed int", 4, true, true );
        _typeStorage.addType< type::Elementary >( "const unsigned int", 4, false, true );
        _typeStorage.addType< type::Elementary >( "const unsigned", 4, false, true );
        _typeStorage.addType< type::Elementary >( "const long", 8, true, true );
        _typeStorage.addType< type::Elementary >( "const long int", 8, true, true );
        _typeStorage.addType< type::Elementary >( "const signed long", 8, true, true );
        _typeStorage.addType< type::Elementary >( "const signed long int", 8, true, true );
        _typeStorage.addType< type::Elementary >( "const unsigned long", 8, false, true );
        _typeStorage.addType< type::Elementary >( "const unsigned long int", 8, false, true );
    }

    AST( const AST & ) = delete;

    AST( AST &&o ) :
        _typeStorage( std::move( o._typeStorage ) ),
        _functions( std::move( o._functions ) ),
        _global( std::move( o._global ) )
    {}

    void add( Function &&f ) {
        std::string name = f.name();
        if ( _functions.count( name ) )
            throw exception::InternalError( "multiple definition of function" );
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

        return _functions.find( name )->second;
    }

    TypeStorage &typeStorage() {
        return _typeStorage;
    }

    const TypeStorage &typeStorage() const {
        return _typeStorage;
    }

private:
    TypeStorage _typeStorage;
    std::map< std::string, Function > _functions;
    Block _global;
};

} // namespace ast
} // namespace compiler
