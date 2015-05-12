#pragma once

#include "Position.h"
#include "Utils.h"
#include "Unicode.h"
#include "Operator.h"
#include "Keyword.h"

#include <string>
#include <iostream>

namespace compiler {
namespace common {

struct Token : Comparable {

    enum class Type {
        Operator,
        Integer,
        Real,
        Keyword,
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
        Keyword keyword;

        _N( long long i ) : integer( i ) {}
        _N( long double r ) : real( r ) {}
        _N( Operator o ) : op( o ) {}
        _N( Keyword k ) : keyword( k ) {}
        _N() = default;
        _N( const _N & ) = default;
    } _detail;

public:

    Token( Type t = Type::Eof, Position p = Position() ) :
        Token( std::string(), t, std::move( p ) )
    {}

    Token( std::string token, Type t = Type::Word, Position p = Position() ) :
        _token( std::move( token ) ),
        _type( t ),
        _position( std::move( p ) )
    {}

    Token( const Token &o ) :
        _token( o._token ),
        _type( o._type ),
        _position( o._position ),
        _detail( o._detail )
    {}

    Token( Token &&o ) :
        _token( std::move( o._token ) ),
        _type( o._type ),
        _position( std::move( o._position ) ),
        _detail( o._detail )
    {}

    void replaceBy( Token o ) {
        swap( o );
        std::swap( _position, o._position );
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

    Keyword &keyword() {
        return _detail.keyword;
    }
    Keyword keyword() const {
        return _detail.keyword;
    }
    bool isKeyword( Keyword k ) const {
        return type() == Type::Keyword && keyword() == k;
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
