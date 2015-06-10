#pragma once

#include "Type.h"

#include <string>

namespace compiler {
namespace code {


struct Register {
    Register( std::string name, Type type ) :
        _name( std::move( name ) ),
        _type( type )
    {}

    Register( const Register & ) = default;
    Register( Register &&other ) :
        _name( std::move( other._name ) ),
        _type( std::move( other._type ) )
    {}

    Register &operator=( Register other ) {
        swap( other );
        return *this;
    }

    void swap( Register &other ) {
        using std::swap;

        swap( _name, other._name );
        swap( _type, other._type );
    }

    const std::string &name() const {
        return _name;
    }

    Type type() const {
        return _type;
    }
private:
    std::string _name;
    Type _type;
};

} // namespace code
} // namespace compiler

namespace std {

template<>
inline void swap< compiler::code::Register >( compiler::code::Register &lhs, compiler::code::Register &rhs ) {
    lhs.swap( rhs );
}

} // namespace std
