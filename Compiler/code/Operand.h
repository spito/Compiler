#pragma once

#include "Type.h"
#include "Register.h"

#include <string>

namespace compiler {
namespace code {

struct Operand {

    Operand() :
        _value( 0 ),
        _type( 0 ),
        _register( false )
    {}

    Operand( Register r ) :
        _name( r.name() ),
        _value( 0 ),
        _type( r.type() ),
        _register( true )
    {}

    Operand( long long value, Type type ) :
        _value( value ),
        _type( type ),
        _register( false )
    {}

    Operand( const Operand & ) = default;
    //Operand( Operand && ) = default;
    Operand( Operand &&other ) :
        _name( std::move( other._name ) ),
        _value( other._value ),
        _type( other._type ),
        _register( other._register )
    {}

    Operand &operator=( Operand other ) {
        swap( other );
        return *this;
    }

    void swap( Operand &other ) {
        using std::swap;

        swap( _name, other._name );
        swap( _value, other._value );
        swap( _register, other._register );
        swap( _type, other._type );
    }

    const std::string &name() const {
        return _name;
    }

    long long value() const {
        return _value;
    }

    const Type &type() const {
        return _type;
    }


    bool isRegister() const {
        return _register;
    }

    static Operand label( int id ) {
        return Operand( id );
    }

    static Operand Void() {
        return Operand( "void" );
    }

private:
    Operand( std::string name ) :
        _name( std::move( name ) ),
        _register( true ),
        _type( 0, false )
    {}

    Operand( int id ) :
        _register( false ),
        _value( id ),
        _type( 0, false )
    {}

    std::string _name;
    long long _value;
    Type _type;
    bool _register;
};

} // namespace code
} // namespace compiler

namespace std {

template<>
inline void swap< compiler::code::Operand >( compiler::code::Operand &lhs, compiler::code::Operand &rhs ) {
    lhs.swap( rhs );
}

} // namespace std
