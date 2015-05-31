#pragma once

#include <utility>
#include <string>
#include <vector>
#include <functional>

namespace compiler {
namespace common {

struct Comparable {
    using IsComparable = bool;
};
struct Orderable {
    using IsOrderable = bool;
};

struct Defer {

    Defer( std::function< void() > &&f ) :
        _f( std::move( f ) ),
        _run( false )
    {}

    Defer( const Defer & ) = delete;
    Defer( Defer &&another ) :
        _f( std::move( another._f ) ),
        _run( another._f )
    {
        another._run = true;
    }

    ~Defer() {
        run();
    }

    void run() {
        if ( !_run ) {
            _f();
            _run = true;
        }
    }

    bool deleted() const {
        return _run;
    }
    void pass() {
        _run = true;
    }

private:
    std::function< void() > _f;
    bool _run;
};

template< typename F >
Defer make_defer( F &&f ) {
    return Defer( std::move( f ) );
}

template< typename T, typename V >
T find_last( T begin, T end, const V &toSearch ) {

    T remember = begin;

    for ( ; begin != end; ++begin ) {
        if ( *begin == toSearch )
            remember = begin;
    }
    return remember;
}

template< typename T >
struct Adaptor {
    Adaptor( T begin, T end ) :
        _begin( begin ),
        _end( end )
    {}

    T begin() {
        return _begin;
    }
    T begin() const {
        return _begin;
    }
    T end() {
        return _end;
    }
    T end() const {
        return _end;
    }

private:
    T _begin;
    T _end;
};

template< typename T >
auto revert( T &&container )
-> Adaptor< decltype( container.rbegin() ) >
{
    using Iterator = decltype( container.rbegin() );
    return Adaptor< Iterator >( container.rbegin(), container.rend() );
}

inline std::string join( const std::vector< std::string > &v, const std::string &glue ) {
    size_t size = v.size() ? ( v.size() - 1 ) * glue.size() : 0;
    for ( const auto &s : v )
        size += s.size();
    std::string result;
    result.reserve( size );

    int i = 0;
    for ( const auto &s : v ) {
        if ( i )
            result += glue;
        result += s;
        ++i;
    }
    return result;
}

} // namespace common
} // namespace compiler

template< typename T, typename R >
auto operator!=( const T &lhs, const R &rhs )
-> typename std::enable_if< std::is_convertible< R, T >::value, typename T::IsComparable >::type
{
    return !( lhs == rhs );
}

template< typename T >
auto operator<=( const T &lhs, const T &rhs )
-> typename T::IsOrderable
{
    return !( rhs < lhs );
}

template< typename T >
auto operator>( const T &lhs, const T &rhs )
-> typename T::IsOrderable
{
    return rhs < lhs;
}

template< typename T >
auto operator>=( const T &lhs, const T &rhs )
-> typename T::IsOrderable
{
    return !( lhs < rhs );
}
