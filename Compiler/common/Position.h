#pragma once

#include <utility>
#include <cstddef>
#include <iostream>
#include <string>

#include "../context/File.h"

namespace compiler {
namespace common {

class Position {
    size_t _rawPosition;
    size_t _line;
    size_t _column;
    const std::string *_file;
public:

    static Position nowhere() {
        return Position( 0, 0, 0, nullptr );
    }

    Position() :
        _rawPosition( 0 ),
        _line( 1 ),
        _column( 1 ),
        _file( &context::file() )
    {}

    Position( const Position & ) = default;
    Position( Position &&o ) :
        _rawPosition( o._rawPosition ),
        _line( o._line ),
        _column( o._column )
    {}

    Position &operator=( Position p ) {
        swap( p );
        return *this;
    }

private:
    Position( size_t p, size_t l, size_t r, const std::string *f ) :
        _rawPosition( p ),
        _line( l ),
        _column( r ),
        _file( f )
    {}

public:

    void nextLine() {
        ++_rawPosition;
        _column = 1;
        ++_line;
    }

    void nextChar() {
        ++_rawPosition;
        ++_column;
    }

    size_t line() const {
        return _line;
    }

    size_t column() const {
        return _column;
    }
    size_t position() const {
        return _rawPosition;
    }
    const std::string &file() const {
        return _file ?
            *_file :
            context::emptyString();
    }

    void reset() {
        _rawPosition = 0;
        _line = 1;
        _column = 1;
        _file = &context::file();
    }

    void swap( Position &p ) {
        using std::swap;
        swap( _rawPosition, p._rawPosition );
        swap( _line, p._line );
        swap( _column, p._column );
    }

    bool operator==( const Position &p ) const {
        return
            _rawPosition == p._rawPosition &&
            _line == p._line &&
            _column == p._column &&
            _file == p._file;
    }
    bool operator!=( const Position &p ) const {
        return !( *this == p );
    }
};
}
}
namespace std {
template<>
inline void swap( ::compiler::common::Position &lhs, ::compiler::common::Position &rhs ) {
    lhs.swap( rhs );
}
}

inline std::ostream &operator<<( std::ostream &out, const ::compiler::common::Position &p ) {
    out << "[" <<
        p.position() << "; " <<
        p.line() << "; " <<
        p.column() << "]";
    return out;
}

