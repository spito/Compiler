#pragma once

#include "Position.h"
#include "Utils.h"
#include "Unicode.h"

#include <string>
#include <iostream>

namespace compiler {
    namespace common {

        struct Token : Comparable {

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
                NewLine,
                FileBegin,
                FileEnd,
                Eof,
            };

            enum class Operator {

                BraceOpen, // {
                BraceClose,

                // rank 1, left to right
                Increment,
                Decrement,
                BracketOpen, // (
                BracketClose,
                BracketIndexOpen, // [
                BracketIndexClose,

                // rank 2, right to left
                UnaryPlus,
                UnaryMinus,
                LogicalNot,
                BitwiseNote,
                //TypeCast, // (int)
                Dereference,
                AddressOf,
                //Sizeof, // sizeof(int)

                // rank 3, left to right
                Multiplication,
                Division,
                Modulo,

                // rank 4, left to right
                Addition,
                Subtraction,

                // rank 5, left to right
                BitwiseLeftShift,
                BitwiseRightShift,

                // rank 6, left to right
                LessThan,
                LessThenOrEqual,
                GreaterThan,
                GreaterThanOrEqual,

                // rank 7, left to right
                EqualTo,
                NotEqualTo,

                // rank 8, left to right
                BitwiseAnd,

                // rank 9, left to right
                BitwiseXor,

                // rank 10, left to right
                BitwiseOr,

                // rank 11, left to right
                LogicalAnd,

                // rank 12, left to right
                LogicalOr,

                // rank 13, right to left
                //TernaryOperator

                // rank 14, right to left
                Assignment,
                AssignmentSum,
                AssignmentDifference,
                AssignmentProduct,
                AssignmentQuotient,
                AssignmentRemainder,
                AssignmentLeftShift,
                AssignmentRightShift,
                AssignmentBitwiseAnd,
                AssignmentBitwiseXor,
                AssignmentBitwiseOr,

                // rank 15, left to right
                Comma,
            };

        private:
            std::string _token;
            Type _type;
            Position _position;

            union _N {
                long long integer;
                long double real;

                _N( long long i ) : integer( i ) {}
                _N( long double r ) : real( r ) {}
                _N() = default;
            } _number;

        public:

            Token( Type t = Type::Eof, Position p = Position() ) :
                Token( std::move( t ), std::string(), std::move( p ) )
            {}
            
            Token( Type t, std::string token, Position p = Position() ) :
                _token( std::move( token ) ),
                _type( t ),
                _position( std::move( p ) )
            {}

            Token( const Token &o ) :
                _token( o._token ),
                _type( o._type ),
                _position( o._position )
            {
                if ( _type == Type::Integer )
                    _number.integer = o._number.integer;
                else if ( _type == Type::Real )
                    _number.real = o._number.real;
            }

            Token( Token &&o ) :
                _token( std::move( o._token ) ),
                _type( o._type ),
                _position( std::move( o._position ) )
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


            std::string &value() {
                return _token;
            }

            const std::string &value() const {
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
                    _token == o._token;
            }

            bool identical( const Token &o ) const {
                return
                    *this == o &&
                    _position == o._position;
            }

        };
        inline std::ostream &operator<<( std::ostream &out, Token token ) {
            if ( token.type() == Token::Type::String )
                return out << '"' << Unicode::toAscii( token.value() ) << '"';
            return out << token.value();
        }
    }
}

namespace std {
    template<>
    inline void swap( ::compiler::common::Token &lhs, ::compiler::common::Token &rhs ) {
        lhs.swap( rhs );
    }
}
