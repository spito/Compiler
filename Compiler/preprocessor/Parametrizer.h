#pragma once

#include "ShadowChunker.h"
#include <vector>
#include <string>

namespace compiler {
namespace preprocessor {

struct Parametrizer {

    Parametrizer( ShadowChunker::Iterator it, bool ignoreInvalid = false ) :
        _it( it ),
        _quit( false ),
        _braces( 0 ),
        _ignore( false ),
        _consumed( 0 ),
        _ignoreInvalid( ignoreInvalid )
    {
        parametrize();
    }

    bool ignored() const {
        return _ignore;
    }

    std::vector< std::vector< common::Token > > result() {
        return std::move( _result );
    }

    int consumed() const {
        return _consumed;
    }

private:

    enum class States {
        Quit,
        Start,
        Inside,
        Param,
        CompoundParam,
        Comma,
        Error
    };

    void parametrize();

    States stStart();
    States stInside();
    States stParam();
    States stCompoundParam();
    States stComma();
    void stError();

    States toInside();
    States toParam();
    States toCompoundParam();
    States toComma();
    States toCompoundParamOpenBrace();
    States toCompoundParamCloseBrace();
    States toError();
    States toQuit();
    States toIgnore();

    bool is( common::Operator );
    bool check();

    void push();

    ShadowChunker::Iterator _it;
    std::vector< std::vector< common::Token > > _result;
    bool _quit;
    int _braces;
    bool _ignore;
    int _consumed;
    bool _ignoreInvalid;
};


} // namespace preprocessor
} // namespace compiler
