#pragma once

#include <utility>

namespace compiler {
    namespace common {

        struct Comparable {};
        struct Orderable {};

        template< typename F >
        struct Defer {

            Defer( F &&f ) :
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

        private:
            F _f;
            bool _run;
        };

        template< typename F >
        Defer< F > make_defer( F &&f ) {
            return Defer< F >( std::move( f ) );
        }

    } // namespace common
} // namespace compiler

template< typename T, typename X >
auto operator!=( const T &lhs, const X &rhs )
-> typename std::enable_if< std::is_base_of< compiler::common::Comparable, T >, bool >::type
{
    return !( lhs == rhs );
}

template< typename X, typename T >
auto operator<=( const X &lhs, const T &rhs )
-> typename std::enable_if< std::is_base_of< compiler::common::Orderable, T >, bool >::type
{
    return !( rhs < lhs );
}

template< typename X, typename T >
auto operator>( const X &lhs, const T &rhs )
-> typename std::enable_if< std::is_base_of< compiler::common::Orderable, T >, bool >::type
{
    return rhs < lhs;
}

template< typename T, typename X >
auto operator>=( const T &lhs, const X &rhs )
-> typename std::enable_if< std::is_base_of< compiler::common::Orderable, T >, bool >::type
{
    return !( lhs < rhs );
}
