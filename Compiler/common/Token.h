#pragma once

#include "Position.h"

#include <string>
#include <iostream>

namespace compiler {
    namespace common {

        struct Token {

            enum class Type {
                Operator,
                Integer,
                Real,
                Word,
                Keyword,
                String,
                Char,
                StringInclude,
                Space,
                Eof,
                Comment,
                None
            };
        private:
            std::string _token;
            Type _type;
            Position _position;
            bool _containsNewLine;

            union _N {
                long long integer;
                long double real;

                _N( long long i ) : integer( i ) {}
                _N( long double r ) : real( r ) {}
                _N() = default;
            } _number;

        public:

            Token( Type t = Type::None, Position p = Position() ) :
                Token( std::move( t ), std::string(), std::move( p ) )
            {}
            
            Token( Type t, std::string token, Position p = Position() ) :
                _token( std::move( token ) ),
                _type( t ),
                _position( std::move( p ) ),
                _containsNewLine( false )
            {}

            Token( const Token &o ) :
                _token( o._token ),
                _type( o._type ),
                _position( o._position ),
                _containsNewLine( o._containsNewLine )
            {
                if ( _type == Type::Integer )
                    _number.integer = o._number.integer;
                else if ( _type == Type::Real )
                    _number.real = o._number.real;
            }

            Token( Token &&o ) :
                _token( std::move( o._token ) ),
                _type( o._type ),
                _position( std::move( o._position ) ),
                _containsNewLine( o._containsNewLine )
            {
                if ( _type == Type::Integer )
                    _number.integer = o._number.integer;
                else if ( _type == Type::Real )
                    _number.real = o._number.real;
            }

            void swap( Token &o ) {
                using std::swap;
                swap( _token, o._token );
                swap( _type, o._type );
                swap( _position, o._position );
                swap( _number, o._number );
                swap( _containsNewLine, o._containsNewLine );
            }


            Token &operator=( Token t ) {
                swap( t );
                return *this;
            }

            size_t size() const {
                return _token.size();
            }

            Type type() const {
                return _type;
            }

            Position &position() {
                return _position;
            }
            const Position &position() const {
                return _position;
            }

            bool &containsNewLine() {
                return _containsNewLine;
            }
            bool containsNewLine() const {
                return _containsNewLine;
            }

            std::string &token() {
                return _token;
            }

            const std::string &token() const {
                return _token;
            }

            long long &integer() {
                return _number.integer;
            }
            long long integer() const {
                return _number.integer;
            }

            long double &real() {
                return _number.real;
            }
            long double real() const {
                return _number.real;
            }

            char &operator[]( size_t index ) {
                return _token[ index ];
            }
            char operator[]( size_t index ) const {
                return _token[ index ];
            }

            auto begin()
                -> decltype( _token.begin() )
            {
                return _token.begin();
            }
            auto end()
                -> decltype( _token.end() )
            {
                return _token.end();
            }
            auto begin() const
                -> decltype( _token.begin() )
            {
                return _token.begin();
            }
            auto end() const
                -> decltype( _token.end() )
            {
                return _token.end();
            }


            bool operator==( const Token &o ) const {
                return
                    _type == o._type &&
                    _position == o._position &&
                    _token == o._token;
            }

            bool operator!=( const Token &o ) const {
                return !( *this == o );
            }
        };
        inline std::ostream &operator<<( std::ostream &out, Token::Type type ) {
            /// TODO: implement
            return out;
        }
        inline std::ostream &operator<<( std::ostream &out, Token token ) {
            return out << token.type();
        }
    }
}

namespace std {
    template<>
    inline void swap( ::compiler::common::Token &lhs, ::compiler::common::Token &rhs ) {
        lhs.swap( rhs );
    }
}
