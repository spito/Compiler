#pragma once

#include "InputBuffer.h"

namespace compiler {
namespace preprocessor {

struct Tokenizer {

    enum class Class {
        None,
        Slash,
        Number,
        Word,
        Space,
        NewLine,
        String,
        ShortComment,
        LongComment,
        Operator,
        Preprocessor,
        Eof,
    };

    Tokenizer( const std::string &name ) :
        _buffer( name )
    {}

    Tokenizer( const Tokenizer & ) = delete;
    Tokenizer( Tokenizer &&other ) :
        _buffer( std::move( other._buffer ) )
    {}

    void restart( const std::string &content ) {
        _buffer.assignContent( content );
    }

    common::Token readToken( bool = false );
    
    void giveBack( const common::Token & );

    const common::Position &position() const {
        return _buffer.position();
    }
    void position( common::Position p ) {
        _buffer.position( std::move( p ) );
    }

private:

    Class resolveClass();

    common::Token getToken( bool );

    common::Token processNumber();
    common::Token processWord();
    common::Token processSpace();
    common::Token processNewLine();
    common::Token processString();
    void processShortComment();
    void processLongComment();
    common::Token processOperator();
    void processSlash();
    common::Token processSharp();

    InputBuffer _buffer;
    bool _readyForCommand = true;

};

} // namespace preprocessor
} // namespace compiler
