#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <cctype>
#include <iterator>
#include <iomanip>

#include "Position.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace common {

class InputBuffer {
    size_t _size;
    std::unique_ptr< char[] > _buffer;
    Position _position;
public:
    InputBuffer( const InputBuffer & ) = delete;
    InputBuffer( InputBuffer &&other ) :
        _size( other._size ),
        _buffer( std::move( other._buffer ) ),
        _position( std::move( other._position ) )
    {}
    InputBuffer( const std::string &content ) :
        _size( content.size() ),
        _buffer( new char[ _size ] )
    {
        std::copy( content.begin(), content.end(), _buffer.get() );
    }

    InputBuffer( const char *content ) :
        _size( strlen( content ) ),
        _buffer( new char[ _size ] )
    {
        std::copy( content, content + _size, _buffer.get() );
    }

    InputBuffer( std::ifstream &file ) :
        _buffer( nullptr )
    {
        readFile( file );
    }

    InputBuffer() :
        _size( 0 ),
        _buffer( nullptr )
    {}

    void assignContent( const std::string &content ) {
        _size = content.size();
        _buffer.reset( new char[ _size ] );
        _position.reset();

        std::copy( content.begin(), content.end(), _buffer.get() );
    }

    void openFile( const char *name ) {
        std::ifstream file( name, std::ios::in );
        _position.reset();
        readFile( file );
    }


    bool eof( size_t pos = 0 ) const {
        return _position.position() + pos >= _size;
    }

    char look( size_t pos = 0 ) const {
        if ( eof( pos ) )
            return '\0';
        return _buffer[ _position.position() + pos ];
    }

    // get only non-white characters
    char readChar() {
        char c;
        do {
            c = read();
        } while ( std::isspace( c ) );
        return c;
    }

    // get all type of characters
    char read() {
        if ( eof() )
            return '\0';

        char c = _buffer[ _position.position() ];

        if ( c == '\n' )
            _position.nextLine();
        else
            _position.nextChar();
        return c;
    }

    void skipWhite() {
        while ( !eof() ) {
            char c = _buffer[ _position.position() ];
            if ( !std::isspace( c ) )
                break;

            if ( c == '\n' )
                _position.nextLine();
            else
                _position.nextChar();
        }
    }

    const Position &position() const {
        return _position;
    }

    void position( Position p ) {
        _position = std::move( p );
    }

private:

    void readFile( std::ifstream &file ) {

        if ( !file.good() )
            throw exception::InvalidFile();

        file.seekg( 0, std::ios::end );
        _size = size_t( file.tellg() );
        file.seekg( 0, std::ios::beg );
        _buffer.reset( new char[ _size ] );

        file >> std::noskipws;

        char *c = std::copy( std::istream_iterator<char>( file ),
                             std::istream_iterator<char>(),
                             _buffer.get() );
        _size = size_t( c - _buffer.get() );
    }
};
}
}
