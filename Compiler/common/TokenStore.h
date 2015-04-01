#pragma once

#include "Token.h"
#include <vector>

namespace compiler {
namespace common {

struct TokenStore {

    TokenStore() :
        _index( 0 )
    {}

    TokenStore( const TokenStore & ) = default;
    TokenStore( TokenStore &&other ) :
        _tokens( std::move( other._tokens ) ),
        _index( other._index )
    {}



    void push( Token &&token ) {
        _tokens.push_back( std::move( token ) );
    }
    void push( std::vector< Token > &tokens ) {
        std::move( tokens.begin(), tokens.end(), std::back_inserter( _tokens ) );
        tokens.clear();
    }

    bool available() {
        return index() < _tokens.size();
    }

    const Token &get() {
        size_t i = index();
        ++_index;
        return _tokens[ i ];
    }

    const Token &get( size_t index ) {
        return _tokens[ this->index() + index ];
    }

    const Token &operator[]( size_t index ) {
        return _tokens[ index ];
    }

    size_t index() {
        while ( _index < _tokens.size() && _tokens[ _index ].type() == common::Token::Type::Space )
            ++_index;
        return _index;
    }

    const Token &look() {
        return _tokens[ index() ];
    }

    size_t size() const {
        return _tokens.size();
    }

    void reset() {
        _index = 0;
    }

private:
    std::vector< Token > _tokens;
    size_t _index;
};


} // namespace common
} // namespace compiler

