#pragma once

#include "../common/Symbol.h"
#include "../includes/exceptions.h"

#include <vector>

namespace compiler {
namespace preprocessor {

struct Symbol : common::Symbol {

    using Base = common::Symbol;
    using Event = std::function < 
        std::vector< common::Token >( const common::Token &, const std::vector< std::vector< common::Token > > * ) > ;

    enum class Kind {
        Nothing,
        Integer,
        Macro,
        Function,
    };

    static Symbol makeSpecial( std::string name, Event ev, int arguments = 0, bool expressionOnly = false ) {
        Symbol s( std::move( name ) );
        s._special = true;
        s._expressionOnly = expressionOnly;
        s._event = std::move( ev );
        if ( arguments ) {
            s._kind = Kind::Function;
            s._parametres.resize( arguments );
        }
        else
            s._kind = Kind::Macro;
        return s;
    }

    Symbol( std::string name ) :
        Base( std::move( name ) ),
        _kind( Kind::Nothing )
    {}

    Symbol( std::string name, common::Token token ) :
        Base( std::move( name ) ),
        _kind( token.type() == common::Token::Type::Integer ? Kind::Integer : Kind::Macro ),
        _integer( token.integer() )
    {
        _value.push_back( std::move( token ) );
    }
    Symbol( std::string name, std::vector< common::Token > value ) :
        Base( std::move( name ) ),
        _kind( Kind::Macro ),
        _value( std::move( value ) )
    {}
    Symbol( std::string name, std::vector< common::Token > parametres, std::vector< common::Token > value ) :
        Base( std::move( name ) ),
        _kind( Kind::Function ),
        _value( std::move( value ) ),
        _parametres( std::move( parametres ) )
    {}

    Symbol( const Symbol & ) = default;
    Symbol( Symbol &&other ) :
        Base( std::move( other ) ),
        _kind( other._kind ),
        _special( other._special ),
        _expressionOnly( other._expressionOnly ),
        _integer( other._integer ),
        _value( std::move( other._value ) ),
        _parametres( std::move( other._parametres ) ),
        _event( std::move( other._event ) )
    {}

    Symbol &operator=( const Symbol & ) = default;
    Symbol &operator=( Symbol &&other ) {
        swap( other );
        return *this;
    }

    Kind kind() const {
        return _kind;
    }

    long long integer() const {
        if ( _kind != Kind::Integer )
            throw exception::InternalError( "Invalid usage of symbol (not an integer)" );
        return _integer;
    }
    const std::vector< common::Token > &value() const {
        return _value;
    }
    const std::vector< common::Token > &parametres() const {
        if ( _kind != Kind::Function )
            throw exception::InternalError( "Invalid usage of symbol (not a function)" );
        return _parametres;
    }

    void swap( Symbol &other ) {
        using std::swap;
        swap( _kind, other._kind );
        swap( _integer, other._integer );
        swap( _value, other._value );
        swap( _parametres, other._parametres );
    }

    bool identical( const Symbol &other ) const {
        if ( kind() != other.kind() || name() != other.name() )
            return false;

        switch ( kind() ) {
        case Symbol::Kind::Nothing:
            return true;
        case Symbol::Kind::Integer:
            return integer() == other.integer();
        case Symbol::Kind::Macro:
            return value() == other.value();
        case Symbol::Kind::Function:
            return value() == other.value() &&
                parametres() == other.parametres();
        default:
            throw exception::InternalError( "Invalid kind of symbol" );
        }
    }

    bool special() const {
        return _special;
    }
    bool expressionOnly() const {
        return _expressionOnly;
    }

    std::vector< common::Token > eval( const common::Token &token, const std::vector< std::vector< common::Token > > *parametres = nullptr ) const {
        return _event( token, parametres );
    }

private:
    Kind _kind;
    long long _integer;
    bool _special = false;
    bool _expressionOnly = false;
    std::vector< common::Token > _value;
    std::vector< common::Token > _parametres;
    Event _event;
};

} // namespace preprocessor
} // namespace compiler
