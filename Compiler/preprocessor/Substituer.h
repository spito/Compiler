#pragma once

#include "../common/SymbolTable.h"
#include "../common/CharClass.h"
#include "ShadowChunker.h"
#include "Symbol.h"
#include "Parametrizer.h"

namespace compiler {
namespace preprocessor {

struct UsedSymbol : common::Symbol {
    //using Base = common::Symbol;
    UsedSymbol( std::string name ) :
        Symbol( std::move( name ) ),
        _arguments( nullptr )
    {}

    UsedSymbol( std::string name, const std::vector< std::vector< std::string > > &arguments ) :
        Symbol( std::move( name ) ),
        _arguments( &arguments )
    {}

    bool function() const {
        return _arguments != nullptr;
    }

    const std::vector< std::vector< std::string > > &arguments() const {
        return *_arguments;
    }

    bool operator==( const UsedSymbol &other ) const {
        return name() == other.name() &&
            ( _arguments == other._arguments || 
            ( _arguments && other._arguments &&
            *_arguments == *other._arguments ) );
    }

private:
    const std::vector< std::vector< std::string > > *_arguments;
};

struct Substituer {
    using SymbolTable = common::SymbolTable < Symbol > ;
    Substituer( std::shared_ptr< SymbolTable > symbols ) :
        _symbols( symbols ),
        _chunks( nullptr )
    {}


    void run( ShadowChunker &chunks ) {
        _used.clear();
        _chunks = &chunks;
        _join = false;
        _stringify = false;
        substitute();
    }

    const std::string &result() const {
        return _result;
    }

private:

    int substitute();

    void prepareForJoin( std::vector< std::string > & );
    void addChunk( std::vector< std::string > &, const std::string & );
    void stringify( std::vector< std::string > &, const std::vector< std::string > & );
    void join( std::vector< std::string > &, const std::vector< std::string > & );
    void recursion( UsedSymbol &&, const std::vector< std::string > &, int = 0 );

    std::string _result;
    std::shared_ptr< SymbolTable > _symbols;
    common::SymbolTable< UsedSymbol > _used;
    ShadowChunker *_chunks;
    bool _join = false;
    bool _stringify = false;

};

} // namespace preprocessor
} // namespace compiler
