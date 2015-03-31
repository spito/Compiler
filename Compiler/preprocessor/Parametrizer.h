#pragma once

#include "ShadowChunker.h"
#include <vector>
#include <string>

namespace compiler {
namespace preprocessor {

struct Parametrizer {

    Parametrizer( ShadowChunker::Iterator it ) :
        _it( it ),
        _quit( false ),
        _braces( 0 ),
        _ignore( false ),
        _consumed( 0 )
    {
        parametrize();
    }

    bool ignored() const {
        return _ignore;
    }

    std::vector< std::vector< std::string > > result() {
        return std::move( _result );
    }

    int consumed() const {
        return _consumed;
    }

private:

    enum class States {
        Quit,
        Start,
        Space,
        Inside,
        SpaceInside,
        Param,
        CompoundParam,
        Comma,
        //CompoundParamEnd,
        Error
    };

    void parametrize();

    States stStart();
    States stSpace();
    States stInside();
    States stSpaceInside();
    States stParam();
    States stCompoundParam();
    States stComma();
    void stError();

    States toSpace();
    States toInside();
    States toSpaceInside();
    States toParam();
    States toCompoundParam();
    States toComma();
    States toCompoundParamOpenBrace();
    States toCompoundParamCloseBrace();
    States toError();
    States toQuit();
    States toIgnore();


    ShadowChunker::Iterator _it;
    std::vector< std::vector< std::string > > _result;
    bool _quit;
    int _braces;
    bool _ignore;
    int _consumed;
};


} // namespace preprocessor
} // namespace compiler
