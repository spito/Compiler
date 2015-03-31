#include "Parametrizer.h"
#include "../common/CharClass.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace preprocessor {


/* states */
auto Parametrizer::stStart() -> States {
    if ( *_it == "(" )
        return toInside();
    if ( common::isSpace( _it->c_str() ) )
        return toSpace();
    return toIgnore();
}

auto Parametrizer::stSpace() -> States {
    if ( *_it == "(" )
        return toInside();
    return toError();
}
auto Parametrizer::stInside() -> States {
    if ( *_it == ")" )
        return toQuit();
    _result.emplace_back(); // open parameter
    if ( common::isSpace( _it->c_str() ) )
        return toSpaceInside();
    if ( *_it == "(" )
        return toCompoundParam();
    if ( *_it == "," )
        return toError();
    return toParam();
}
auto Parametrizer::stSpaceInside() -> States {
    if ( *_it == "," )
        return toError();
    if ( *_it == "(" )
        return toCompoundParam();
    return toParam();
}
auto Parametrizer::stParam() -> States {
    if ( *_it == "," )
        return toComma();
    if ( *_it == ")" )
        return toQuit();
    if ( *_it == "(" )
        return toCompoundParam();
    return toParam();
}
auto Parametrizer::stCompoundParam() -> States {
    if ( *_it == "(" )
        return toCompoundParamOpenBrace();
    if ( _braces && *_it == ")" )
        return toCompoundParamCloseBrace();
    if ( *_it == ")" )
        return toParam();
    return toCompoundParam();
}
auto Parametrizer::stComma() -> States {
    if ( *_it == "," || *_it == ")" )
        return toError();
    _result.emplace_back(); // open parameter
    if ( *_it == "(" )
        return toCompoundParam();
    if ( common::isSpace( _it->c_str() ) )
        return toSpaceInside();
    return toParam();
}
void Parametrizer::stError() {
    throw exception::InternalError( "malformed input" );
}
/* transitions */
auto Parametrizer::toSpace() -> States {
    return States::Space;
}
auto Parametrizer::toInside() -> States {
    return States::Inside;
}
auto Parametrizer::toSpaceInside() -> States {
    return States::SpaceInside;
}
auto Parametrizer::toParam() -> States {
    _result.back().push_back( *_it );
    return States::Param;
}
auto Parametrizer::toCompoundParam() -> States {
    _result.back().push_back( *_it );
    return States::CompoundParam;
}
auto Parametrizer::toComma() -> States {
    if ( common::isSpace( _result.back().back().c_str() ) )
        _result.back().pop_back();
    return States::Comma;
}
auto Parametrizer::toCompoundParamOpenBrace() ->States {
    _result.back().push_back( *_it );
    ++_braces;
    return States::CompoundParam;
}
auto Parametrizer::toCompoundParamCloseBrace() ->States {
    _result.back().push_back( *_it );
    --_braces;
    return States::CompoundParam;
}
auto Parametrizer::toError() -> States {
    return States::Error;
}
auto Parametrizer::toQuit() -> States {
    if ( !_result.empty() &&
         !_result.back().empty() &&
         common::isSpace( _result.back().back().c_str() ) ) {
        _result.back().pop_back();
    }
    _quit = true;
    return States::Quit;
}
auto Parametrizer::toIgnore() -> States {
    _ignore = true;
    _quit = true;
    return States::Quit;
}

void Parametrizer::parametrize() {
    States state = States::Start;
    for ( ; !_quit; ++_it, ++_consumed ) {
        switch ( state ) {
        case States::Start: state = stStart(); break;
        case States::Space: state = stSpace(); break;
        case States::Inside: state = stInside(); break;
        case States::Param: state = stParam(); break;
        case States::SpaceInside: state = stSpaceInside(); break;
        case States::Comma: state = stComma(); break;
        case States::CompoundParam: state = stCompoundParam(); break;
        case States::Quit: break;
        default:
        case States::Error: stError(); break;
        }
    }

}

} // namespace preprocessor
} // namespace compiler
