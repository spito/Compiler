#pragma once

#include "Utils.h"

#include <string>
#include <functional>
#include <memory>

namespace compiler {
namespace common {

struct Symbol : Comparable {

    Symbol( std::string name ) :
        _name( std::move( name ) )
    {}
    Symbol( const Symbol & ) = default;
    Symbol( Symbol &&other ) :
        _name( std::move( other._name ) )
    {}
    Symbol &operator=( const Symbol & ) = default;
    Symbol &operator=( Symbol &&other ) {
        using std::swap;
        swap( _name, other._name );
        return *this;
    }

    //virtual ~Symbol() = default;

    const std::string &name() const {
        return _name;
    }

    size_t hash() const {
        return std::hash< std::string >()( _name );
    }

    bool operator==( const Symbol &another ) const {
        return _name == another._name;
    }

private:
    std::string _name;
};

} // namespace common
} // namespace compiler
