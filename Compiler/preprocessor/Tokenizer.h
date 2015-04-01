#pragma once

#include "Tokens.h"
#include "../common/InputBuffer.h"

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

    bool isTokenType( common::Token &, const common::Token::Type );

    // destroying
    void readToken( common::Token &, bool = false );

    // not-destroying
    void lookAtToken( common::Token &, bool = false );

    const common::Position &position() const {
        return _input.position();
    }
    void position( common::Position p ) {
        _input.position( std::move( p ) );
    }

private:

    Class resolveClass();

    void getToken( common::Token &, bool );

    void processNumber( common::Token & );
    void processWord( common::Token & );
    void processSpace( common::Token & );
    void processNewLine( common::Token & );
    void processString( common::Token & );
    void processShortComment();
    void processLongComment();
    void processOperator( common::Token & );
    void processSlash();
    void processSharp( common::Token & );

    common::InputBuffer _input;
    bool _readyForCommand = true;

};

} // namespace preprocessor
} // namespace compiler
