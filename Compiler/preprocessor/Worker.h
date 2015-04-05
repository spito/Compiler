#pragma once

#include "Global.h"
#include "Tokenizer.h"

#include <stack>
#include <map>

namespace compiler {
namespace preprocessor {

struct ConditionFrame {
    bool ignore = false;
    bool fulfilled = false;
    bool inherited = false;
    bool exhausted = false;
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
    std::stack< ConditionFrame > _stack;

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

