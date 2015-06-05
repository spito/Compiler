#pragma once

#include "Type.h"
#include "Register.h"

#include <string>

namespace compiler {
namespace code {

struct Operand {

    Operand( Register r ) :
        _name( r.name() ),
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
