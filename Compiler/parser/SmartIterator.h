#pragma once

#include "../common/SmartIterator.h"

namespace compiler {
namespace parser {

struct SmartIterator : common::SmartIterator < std::vector< common::Token >::iterator > {
    using Iterator = std::vector< common::Token >::iterator;
    using Base = common::SmartIterator< Iterator >;
    using Type = common::Token::Type;

    SmartIterator( Iterator begin, Iterator end ) :
        Base( begin, end )
    {}

    SmartIterator( const Base &base ) :
        Base( base )
    {}

    SmartIterator( const SmartIterator & ) = default;
    SmartIterator &operator=( const SmartIterator & ) = default;

    explicit operator bool() const {
        return
            _begin != _end &&
            _begin->type() != Type::Eof;
    }
    SmartIterator &operator++( ) {
        do { ++_begin; } while ( skip() );
        return *this;
    }
    SmartIterator operator++( int ) {
        SmartIterator self( *this );
        do { ++_begin; } while ( skip() );
        return self;
    }
    SmartIterator &operator--( ) {
        do { --_begin; } while ( skip() );
        return *this;
    }
    SmartIterator operator--( int ) {
        SmartIterator self( *this );
        do { --_begin; } while ( skip() );
        return self;
    }
private:
    bool skip() {
        return
            _begin->type() == Type::FileBegin ||
            _begin->type() == Type::FileEnd;
    }

};

} // namespace parser
} // namespace compiler
