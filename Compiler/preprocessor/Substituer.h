#pragma once

#include "../common/SymbolTable.h"
#include "../common/CharClass.h"
#include "ShadowChunker.h"
#include "Symbol.h"
#include "Parametrizer.h"
#include "Tokenizer.h"

namespace compiler {
namespace preprocessor {

struct UsedSymbol : common::Symbol {

    UsedSymbol( common::Token token ) :
        Symbol( token.value() ),
        _token( token ),
        _arguments( nullptr )
        {}

    UsedSymbol( common::Token token, const std::vector< std::vector< common::Token > > &arguments ) :
        Symbol( token.value() ),
        _token( token ),
        _arguments( &arguments )
    {}

    bool function() const {
        return _arguments != nullptr;
    }

    const std::vector< std::vector< common::Token > > &arguments() const {
        return *_arguments;
    }

    const common::Token &token() const {
        return _token;
    }

    bool operator==( const UsedSymbol &other ) const {
        return name() == other.name() &&
            ( _arguments == other._arguments || 
            ( _arguments && other._arguments &&
            *_arguments == *other._arguments ) );
    }

private:
    common::Token _token;
    const std::vector< std::vector< common::Token > > *_arguments;
};

struct Substituer {
    using SymbolTable = common::SymbolTable < Symbol > ;
    Substituer( SymbolTable &symbols, common::Token initial, Tokenizer &tokenizer ) :
        _position( initial.position() ),
        _symbols( symbols ),
        _chunks( std::move( initial ), [&] { return tokenizer.readToken(); } ),
        _join( false ),
        _stringify( false )
    {
        substitute();

        for ( auto &token : _result )
            token.position() = _position;
    }

    std::vector< common::Token > &result() {
        return _result;
    }


private:

    int substitute();

    void prepareForJoin( std::vector< common::Token > & );
    void addChunk( std::vector< common::Token > &, const common::Token & );
    void stringify( std::vector< common::Token > &, const std::vector< common::Token > & );
    void join( std::vector< common::Token > &, const std::vector< common::Token > & );
    void recursion( UsedSymbol &&, const std::vector< common::Token > &, int = 0 );

    common::Position _position;
    std::vector< common::Token > _result;
    SymbolTable &_symbols;
    common::SymbolTable< UsedSymbol > _used;
    ShadowChunker _chunks;
    bool _join = false;
    bool _stringify = false;

};

} // namespace preprocessor
} // namespace compiler
