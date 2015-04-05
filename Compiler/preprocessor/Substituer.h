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
        _tokenizer( &tokenizer ),
        _chunks( ShadowChunker( std::move( initial ), [&] { return tokenizer.readToken(); } ) ),
        _symbols( symbols )
    {
        _result = substitute();
    }
    Substituer( SymbolTable &symbols,
                std::vector< common::Token >::const_iterator begin,
                std::vector< common::Token >::const_iterator end ) :
        _tokenizer( nullptr ),
        _symbols( symbols ),
        _chunks( ShadowChunker() ),
        _insideExpression( true )
    {
        _chunks.prepend( begin, end );
        auto diff = end - begin;
        for ( int eaten = 0; eaten < diff; ) {
            int consumed;
            merge( _result, substitute( &consumed ) );
            eaten += consumed;
        }
    }

    std::vector< common::Token > &result() {
        return _result;
    }

private:

    Substituer( const Substituer &self,
                std::vector< common::Token >::const_iterator begin,
                std::vector< common::Token >::const_iterator end ) :
        _tokenizer( nullptr ),
        _symbols( self._symbols ),
        _used( self._used ),
        _chunks( ShadowChunker() )
    {
        _chunks.prepend( begin, end );
        _result = substitute();
    }


    std::vector< common::Token > substitute( int * = nullptr );
    std::vector< common::Token > substituteMacro( common::Token, const Symbol &, int * );
    std::vector< common::Token > substituteInteger( common::Token, const Symbol &, int * );
    std::vector< common::Token > substituteFunction( common::Token, const Symbol &, int * );
    std::vector< common::Token > substituteSpecial( const common::Token &, const Symbol &, int * );




    void addChunk( std::vector< common::Token > &, const common::Token & );
    void stringify( std::vector< common::Token > &, const std::vector< common::Token > & );
    void join( std::vector< common::Token > &, const std::vector< common::Token > & );
    std::vector< common::Token > recursion( UsedSymbol &&, std::vector< common::Token > );

    void merge( std::vector< common::Token > &, std::vector< common::Token > & );

    bool limited() const {
        return !_tokenizer;
    }
    void savePosition() {
        if ( !limited() )
            _savedPosition = _tokenizer->position();
    }
    void restorePosition() {
        if ( !limited() )
            _tokenizer->position( _savedPosition );
    }

    Tokenizer *_tokenizer;
    SymbolTable &_symbols;
    common::SymbolTable< UsedSymbol > _used;
    ShadowChunker _chunks;
    bool _join = false;
    bool _stringify = false;
    bool _insideExpression = false;
    std::vector< common::Token > _result;
    common::Position _savedPosition;
};

} // namespace preprocessor
} // namespace compiler
