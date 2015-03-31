#pragma once

#include "../common/Utils.h"

#include <deque>
#include <string>
#include <vector>
#include <functional>

namespace compiler {
namespace preprocessor {

struct ShadowChunker {

    struct Iterator :
        std::iterator< std::random_access_iterator_tag, std::string >,
        common::Comparable,
        common::Orderable 
    {

        Iterator() :
            _chunks( nullptr ),
            _fetcher( [] { return ""; } )
        {}
        Iterator( std::deque< std::string > &chunks, std::deque< std::string >::iterator it, std::function< std::string() > fetcher ) :
            _chunks( &chunks ),
            _it( it ),
            _fetcher( fetcher )
        {}
        Iterator( const Iterator & ) = default;
        //Iterator( Iterator && ) = default;

        Iterator &operator++() {
            increment();
            return *this;
        }
        Iterator operator++( int ) {
            Iterator self( *this );
            increment();
            return self;
        }
        Iterator &operator--( ) {
            --_it;
            return *this;
        }
        Iterator operator--( int ) {
            Iterator self( *this );
            --_it;
            return self;
        }

        Iterator &operator+=( difference_type n ) {
            increment( n );
            return *this;
        }
        Iterator &operator-=( difference_type n ) {
            return *this += -n;
        }
        difference_type operator-( Iterator i ) const {
            return _it - i._it;
        }

        std::string &operator*( ) {
            checkValidity();
            return *_it;
        }
        std::string *operator->( ) {
            checkValidity();
            return _it.operator->( );
        }
        std::string &operator[]( difference_type n ) {
            Iterator self( *this );
            return *( self += n );
        }

        bool valid() const {
            return _chunks != nullptr;
        }
        explicit operator bool() const {
            return valid();
        }

        bool operator==( const Iterator &other ) {
            return _chunks == other._chunks &&
                _it == other._it;
        }
        bool operator<( const Iterator &other ) {
            return _it < other._it;
        }

    private:
        void checkValidity() {
            if ( _it == _chunks->end() )
                increment();
        }
        void increment( difference_type n = 1 ) {
            bool wasEnd = _it == _chunks->end();
            if ( _chunks ) {
                difference_type inc = n - ( _chunks->end() - _it );
                while ( inc--> 0 )
                    _chunks->push_back( _fetcher() );
            }
            if ( wasEnd )
                _it = --_chunks->end();
            else
                _it += n;
        }


        std::deque< std::string > *_chunks;
        std::deque< std::string >::iterator _it;
        std::function< std::string() > _fetcher;

    };

    ShadowChunker( std::string initial, std::function< std::string() > fetcher ) :
        _fetcher( std::move( fetcher ) )
    {
        _chunks.push_back( std::move( initial ) );
    }

    const std::string &top() const {
        return _chunks.front();
    }

    void pop() {
        _chunks.pop_front();
    }

    bool empty() const {
        return _chunks.empty();
    }

    void pop( int n ) {
        _chunks.erase( _chunks.begin(), _chunks.begin() + n );
    }

    void prepend( const std::vector< std::string > &items ) {
        _chunks.insert( _chunks.begin(), items.begin(), items.end() );
    }

    Iterator begin() {
        return Iterator( _chunks, _chunks.begin(), _fetcher );
    }
    Iterator end() {
        return Iterator( _chunks, _chunks.end(), _fetcher );
    }

private:
    std::deque< std::string > _chunks;
    std::function< std::string() > _fetcher;



};

inline ShadowChunker::Iterator operator+( ShadowChunker::Iterator it, ShadowChunker::Iterator::difference_type n ) {
    return it += n;
}
inline ShadowChunker::Iterator operator+( ShadowChunker::Iterator::difference_type n, ShadowChunker::Iterator it ) {
    return it += n;
}
inline ShadowChunker::Iterator operator-( ShadowChunker::Iterator it, ShadowChunker::Iterator::difference_type n ) {
    return it -= n;
}
inline ShadowChunker::Iterator operator-( ShadowChunker::Iterator::difference_type n, ShadowChunker::Iterator it ) {
    return it -= n;
}

} // namespace preprocessor
} // namespace compiler

