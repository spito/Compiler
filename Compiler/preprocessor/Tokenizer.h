#pragma once

#include "Tokens.h"
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
    };

    Tokenizer( std::ifstream &file ) :
        _input( file )
    {}

    Tokenizer( const Tokenizer & ) = delete;
    Tokenizer( Tokenizer &&other ) :
        _input( std::move( other._input ) )
    {}

    void restart( const std::string &content ) {
        _input.assignContent( content );
    }

    // destroying
    common::Token readToken( bool = false );
    
    // not-destroying
    common::Token lookAtToken( bool = false );

    const common::Position &position() const {
        return _input.position();
    }
    void position( common::Position p ) {
        _input.position( std::move( p ) );
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

    InputBuffer _input;
    bool _readyForCommand = true;

};

} // namespace preprocessor
} // namespace compiler
