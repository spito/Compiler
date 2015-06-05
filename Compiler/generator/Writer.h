#pragma once

#include "../code/Code.h"

#include <fstream>
#include <string>

namespace compiler {
namespace generator {

struct Writer {

    virtual void writeInstruction( const code::Instruction & ) = 0;
    virtual void writeBlock( const code::BasicBlock & ) = 0;
    virtual void writeFunction( const code::Function & ) = 0;

protected:

    Writer( const char *filename ) : 
        _file( filename )
    {}

    Writer( const std::string &filename ) :
        _file( filename.c_str() )
    {}

    std::ofstream &write() {
        return _file;
    }

    template< typename... Args >
    void writeFormatted( const char *fmt, Args &&... args ) {
        _writeFormatted( fmt, std::forward< Args >( args )... );
    }
    template< typename... Args >
    void writeFormattedLine( const char *fmt, Args &&... args ) {
        _writeFormatted( fmt, std::forward< Args >( args )... );
        _file << std::endl;
    }

private:
    std::ofstream _file;

    template< typename T, typename... Args >
    void _writeFormatted( const char *fmt, T &&arg, Args &&... args ) {
        for ( ; *fmt && *fmt != '#'; ++fmt ) {
            _file << *fmt;
        }

        if ( *fmt ) {
            _file << arg;
            ++fmt;
            _writeFormatted( fmt, std::forward< Args >( args )... );
        }
    }


    void _writeFormatted( const char *fmt ) {
        _file << fmt;
    }
};


} // namespace generator
} // namespace compiler
