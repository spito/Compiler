#include "Parametrizer.h"
#include "../common/CharClass.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace preprocessor {

using Operator = common::Operator;


bool Parametrizer::is( Operator op ) {
    return _it->isOperator( op );
}
bool Parametrizer::check() {
    return _it->type() != common::Token::Type::Eof;
}

void Parametrizer::push() {
    //common::Token token( *_it );
    //if ( token.type() == common::Token::Type::Word )
    //    token.type() = common::Token::Type::Special;
    _result.back().push_back( *_it );
}


/* states */
auto Parametrizer::stStart() -> States {
    if ( is( Operator::BracketOpen ) )
        return toInside();
    return toIgnore();
}

auto Parametrizer::stInside() -> States {
    if ( !check() )
        return toError();
    if ( is( Operator::BracketClose ) )
        return toQuit();
    _result.emplace_back(); // open parameter

    if ( is( Operator::BracketOpen ) )
        return toCompoundParam();
    if ( is( Operator::Comma ) )
        return toError();
    return toParam();
}

auto Parametrizer::stParam() -> States {
    if ( !check() )
        return toError();
    if ( is( Operator::Comma ) )
        return toComma();
    if ( is( Operator::BracketClose ) )
        return toQuit();
    if ( is( Operator::BracketOpen ) )
        return toCompoundParam();
    return toParam();
}

auto Parametrizer::stCompoundParam() -> States {
    if ( !check() )
        return toError();
    if ( is( Operator::BracketOpen ) )
        return toCompoundParamOpenBrace();
    if ( _braces && is( Operator::BracketClose ) )
        return toCompoundParamCloseBrace();
    if ( is( Operator::BracketClose ) )
        return toParam();
    return toCompoundParam();
}
auto Parametrizer::stComma() -> States {
    if ( !check() || is( Operator::Comma ) || is( Operator::BracketClose ) )
        return toError();
    _result.emplace_back(); // open parameter

    if ( is( Operator::BracketOpen ) )
        return toCompoundParam();
    return toParam();
}
void Parametrizer::stError() {
    throw exception::InternalError( "malformed input" );
}
/* transitions */
auto Parametrizer::toInside() -> States {
    return States::Inside;
}
auto Parametrizer::toParam() -> States {
    push();
    return States::Param;
}
auto Parametrizer::toCompoundParam() -> States {
    push();
    return States::CompoundParam;
}
auto Parametrizer::toComma() -> States {
    return States::Comma;
}
auto Parametrizer::toCompoundParamOpenBrace() ->States {
    push();
    ++_braces;
    return States::CompoundParam;
}
auto Parametrizer::toCompoundParamCloseBrace() ->States {
    push();
    --_braces;
    return States::CompoundParam;
}
auto Parametrizer::toError() -> States {
    if ( _ignoreInvalid )
        return toIgnore();
    return States::Error;
}
auto Parametrizer::toQuit() -> States {
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
        case States::Inside: state = stInside(); break;
        case States::Param: state = stParam(); break;
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
