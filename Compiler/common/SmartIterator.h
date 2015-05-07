#pragma once

#include "Utils.h"

namespace compiler {
namespace common {

template< typename _Iterator >
struct SmartIterator : Comparable, Orderable {
    using Iterator = _Iterator;
    using Value = typename std::remove_reference< decltype( *Iterator() ) >::type;

    template< typename AnotherIterator >
    SmartIterator( const SmartIterator< AnotherIterator > &another ) :
        _begin( another._begin ),
        _end( another._end )
    {}
    SmartIterator( Iterator begin, Iterator end ) :
        _begin( begin ),
        _end( end )
    {}
    SmartIterator( const SmartIterator & ) = default;
    SmartIterator &operator=( const SmartIterator & ) = default;

    explicit operator bool() const {
        return _begin != _end;
    }

    ptrdiff_t diff() const {
        return _end - _begin;
    }

    SmartIterator &operator++( ) {
        ++_begin;
        return *this;
    }
    SmartIterator operator++( int ) {
        SmartIterator self( *this );
        ++_begin;
        return self;
    }
    SmartIterator &operator--( ) {
        --_begin;
        return *this;
    }
    SmartIterator operator--( int ) {
        SmartIterator self( *this );
        --_begin;
        return self;
    }


    Iterator operator->( ) {
        return _begin;
    }
    Iterator operator->( ) const {
        return _begin;
    }
    const Value &operator*( ) const {
        return *_begin;
    }
    Value &operator*( ) {
        return *_begin;
    }

    bool operator==( const SmartIterator &other ) const {
        return _begin == other._begin;
    }
    bool operator<( const SmartIterator &other ) const {
        return _begin < other._begin;
    }
        
protected:
    Iterator _begin;
    Iterator _end;
};


} // namespace common
} // namespace compiler
