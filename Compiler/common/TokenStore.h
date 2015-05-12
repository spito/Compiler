#pragma once

#include "Token.h"
#include "SmartIterator.h"
#include "Keyword.h"

#include <vector>
#include <unordered_map>

namespace compiler {
namespace common {

struct TokenStore {

    using Iterator = SmartIterator< std::vector< Token >::iterator > ;
    using ConstIterator = SmartIterator < std::vector< Token >::const_iterator >;

    TokenStore()
    {}

    TokenStore( const TokenStore & ) = default;
    TokenStore( TokenStore &&other ) :
        _tokens( std::move( other._tokens ) )
    {}



    void push( Token &&token ) {
        if ( !_tokens.empty() &&
             _tokens.back().type() == Token::Type::String &&
             token.type() == Token::Type::String )
        {
            _tokens.back().value() += token.value();
        }
        else {
            if ( token.type() == Token::Type::Word ) {
                auto i = _keywords.find( token.value() );
                if ( i != _keywords.end() ) {
                    token.replaceBy( Token( std::move( token.value() ), Token::Type::Keyword ) );
                    token.keyword() = i->second;
                }
            }
            _tokens.push_back( std::move( token ) );
        }
    }
    void push( std::vector< Token > &tokens ) {
        for ( auto &token : tokens ) {
            push( std::move( token ) );
        }
        tokens.clear();
    }

    size_t size() const {
        return _tokens.size();
    }

    Iterator begin() {
        return{ _tokens.begin(), _tokens.end() };
    }
    ConstIterator begin() const {
        return{ _tokens.begin(), _tokens.end() };
    }
    ConstIterator cbegin() const {
        return{ _tokens.begin(), _tokens.end() };
    }

    Iterator end() {
        return{ _tokens.end(), _tokens.end() };
    }
    ConstIterator end() const {
        return{ _tokens.end(), _tokens.end() };
    }
    ConstIterator cend() const {
        return{ _tokens.end(), _tokens.end() };
    }

private:
    std::vector< Token > _tokens;

    static std::unordered_map< std::string, Keyword > _keywords;
};


} // namespace common
} // namespace compiler

