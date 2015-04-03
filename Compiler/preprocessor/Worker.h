#pragma once

#include "Global.h"
#include "Tokenizer.h"

#include <stack>
#include <map>

namespace compiler {
namespace preprocessor {

struct ConditionalFrame {
    common::Token lastUsed;
    bool ignore;
    bool fulfilled;
    bool inherited;
    bool exhausted;

    ConditionalFrame( common::Token lastUsed ) :
        lastUsed( std::move( lastUsed ) ),
        ignore( false ),
        fulfilled( false ),
        inherited( false ),
        exhausted( false )
    {}
    ConditionalFrame( const ConditionalFrame & ) = delete;
    ConditionalFrame( ConditionalFrame &&other ) :
        lastUsed( std::move( other.lastUsed ) ),
        ignore( other.ignore ),
        fulfilled( other.fulfilled ),
        inherited( other.inherited ),
        exhausted( other.exhausted )
    {}
};

struct Worker {

    Worker( const std::string &name, Global &global ) :
        _global( global ),
        _tokenizer( std::ifstream( name.c_str() ) ),
        _name( &context::file() )
    {
        context::position( &position() );
        processText();
    }
private:

    Global &_global;
    Tokenizer _tokenizer;
    bool _ready = true;
    const std::string *_name;
    std::stack< ConditionalFrame > _stack;

    static std::map< std::string, void( Worker::* )( void ) > _keywords;

    void processText();
    void invokeExpression();

    void processDefine();
    void processUndef();
    void processPragma();
    void processInclude();

    void processIfdef();
    void processIfndef();
    void processElse();
    void processEndif();

    const common::Position &position() const {
        return _tokenizer.position();
    }
    void position( common::Position p ) {
        _tokenizer.position( std::move( p ) );
    }

    common::SymbolTable< Symbol > &symbols() {
        return _global.symbols;
    }

    common::TokenStore &store() {
        return _global.store;
    }

    bool ignore() const {
        return !_stack.empty() && _stack.top().ignore;
    }

};


} // namespace preprocessor
} // namespace compiler

