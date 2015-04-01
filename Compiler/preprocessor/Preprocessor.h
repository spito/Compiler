#pragma once

#include "../common/SymbolTable.h"
#include "../common/TokenStore.h"
#include "Tokenizer.h"
#include "Symbol.h"

#include <string>
#include <memory>

namespace compiler {
namespace preprocessor {

struct Preprocessor {

    static Preprocessor start( const std::string & );

    size_t size() const {
        return _store.size();
    }

    const common::TokenStore &store() const {
        return _store;
    }
    common::TokenStore &store() {
        return _store;
    }

    Preprocessor( const Preprocessor & ) = delete;
    Preprocessor( Preprocessor &&other ) :
        _store( std::move( other._store ) ),
        _tokenizer( std::move( other._tokenizer ) ),
        _symbols( std::move( other._symbols ) )
    {}


private:

    Preprocessor( const std::string &, std::shared_ptr< common::SymbolTable< Symbol > > );
    Preprocessor( const std::string & );


    void processText();
    void invokeExpression();

    void processDefine();
    void processUndef();
    void processPragma();

    void includeFile( std::string );
    void includeSystemFile( std::string );
    void setPredefined();

    const common::Position &position() const {
        return _tokenizer.position();
    }
    void position( common::Position p ) {
        _tokenizer.position( std::move( p ) );
    }

    common::TokenStore _store;

    Tokenizer _tokenizer;
    std::shared_ptr< common::SymbolTable< Symbol > > _symbols;
    bool _ready = true;
    const std::string *_name;

};

}

}
