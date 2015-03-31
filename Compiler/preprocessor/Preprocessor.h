#pragma once

#include "../common/SymbolTable.h"
#include "Tokenizer.h"
#include "Symbol.h"

#include <string>
#include <memory>

namespace compiler {
namespace preprocessor {

struct Preprocessor {

    Preprocessor( const std::string & );


    void saveToFile( const std::string & ) const;
    size_t size() const {
        return _content.size();
    }
    const char *content() const {
        return _content.c_str();
    }

    Preprocessor( const Preprocessor & ) = delete;
    Preprocessor( Preprocessor &&other ) :
        _content( std::move( other._content ) ),
        _tokenizer( std::move( other._tokenizer ) ),
        _symbols( std::move( other._symbols ) )
    {}


private:

    Preprocessor( const std::string &, std::shared_ptr< common::SymbolTable< Symbol > > );

    void nextPhase();

    void removeComments();

    void processText();
    void invokeExpression();

    void processDefine();
    void processUndef();
    void processPragma();

    void includeFile( std::string );
    void includeSystemFile( std::string );
    void setPredefined();

    common::Position position() const {
        return _tokenizer.position();
    }
    void position( common::Position p ) {
        _tokenizer.position( std::move( p ) );
    }

    std::string _content;

    Tokenizer _tokenizer;
    std::shared_ptr< common::SymbolTable< Symbol > > _symbols;
    bool _ready;

};

}

}
