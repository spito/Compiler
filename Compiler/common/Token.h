#pragma once

#include "Position.h"
#include "Utils.h"
#include "Unicode.h"
#include "Operator.h"

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
        String,
        Char,
        StringInclude,
        Space,
        NewLine,
        FileBegin,
        FileEnd,
        Eof,
    };


private:
    std::string _token;
    Type _type;
    Position _position;

    union _N {
        long long integer;
        long double real;
        Operator op;

        _N( long long i ) : integer( i ) {}
        _N( long double r ) : real( r ) {}
        _N( Operator o ) : op( o ) {}
        _N() = default;
    } _detail;

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
            integer() = o.integer();
        else if ( _type == Type::Real )
            real() = o.real();
        else if ( _type == Type::Operator )
            op() = o.op();
    }

    Token( Token &&o ) :
        _token( std::move( o._token ) ),
        _type( o._type ),
        _position( std::move( o._position ) )
    {
        if ( _type == Type::Integer )
            integer() = o.integer();
        else if ( _type == Type::Real )
            real() = o.real();
        else if ( _type == Type::Operator )
            op() = o.op();
    }

    void swap( Token &o ) {
        using std::swap;
        swap( _token, o._token );
        swap( _type, o._type );
        swap( _position, o._position );
        swap( _detail, o._detail );
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
        return _detail.integer;
    }
    long long integer() const {
        return _detail.integer;
    }

    long double &real() {
        return _detail.real;
    }
    long double real() const {
        return _detail.real;
    }

    Operator &op() {
        return _detail.op;
    }
    Operator op() const {
        return _detail.op;
    }
    bool isOperator( Operator o ) const {
        return type() == Type::Operator && op() == o;
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
inline std::ostream &operator<<( std::ostream &out, const Token &token ) {
    if ( token.type() == Token::Type::String )
        return out << '"' << Unicode::toAscii( token.value() ) << '"';
    return out << token.value();
}

} // namespace common
} // namespace compiler

namespace std {
template<>
inline void swap( ::compiler::common::Token &lhs, ::compiler::common::Token &rhs ) {
    lhs.swap( rhs );
}
} // namespace std
