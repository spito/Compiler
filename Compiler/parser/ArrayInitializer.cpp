#include "ArrayInitializer.h"
#include "Expression.h"

#include <vector>
#include <numeric>
#include <algorithm>

namespace compiler {
namespace parser {

void ArrayInitializer::dimensions() {
    _dimensions.clear();
    const ast::type::Type *t = _type;

    while ( t->kind() == ast::type::Kind::Array ) {
        const ast::type::Array *a = t->as< ast::type::Array >();
        t = &a->of();
        _dimensions.push_back( a->count() );
    }
    _baseType = t;
}

auto ArrayInitializer::stStart() -> States {
    if ( _it->type() == common::Token::Type::String )
        return beString();
    if ( _it->isOperator( common::Operator::BraceOpen ) )
        return toFirstBraceOpen();
    return toError();
}

auto ArrayInitializer::stBraceOpen() -> States {
    if ( _it->isOperator( Operator::BraceOpen ) )
        return toFirstBraceOpen();
    if ( _it->isOperator( Operator::BraceClose ) )
        return toBraceClose();
    if ( _it->type() == common::Token::Type::String )
        return toStringValue();
    return toValue();
}

auto ArrayInitializer::stValue() -> States {
    if ( _it->isOperator( Operator::BraceClose ) )
        return toBraceClose();
    if ( _it->isOperator( Operator::Comma ) )
        return toCommaInside();
    return toError();
}

auto ArrayInitializer::stCommaInside() -> States {
    if ( _it->isOperator( Operator::BraceClose ) )
        return toBraceClose();
    if ( _it->type() == common::Token::Type::String )
        return toStringValue();
    return toValue();
}

auto ArrayInitializer::stCommaOutside() -> States {
    if ( _it->isOperator( Operator::BraceClose ) )
        return toBraceClose();
    if ( _it->isOperator( Operator::BraceOpen ) )
        return toNextBraceOpen();
    if ( _it->type() == common::Token::Type::Integer )
        return toValue();
    return toError();
}

auto ArrayInitializer::stBraceClose() -> States {
    if ( _currentIndex.empty() )
        return toQuit();
    if ( _it->isOperator( Operator::Comma ) )
        return toCommaOutside();
    if ( _it->isOperator( Operator::BraceClose ) )
        return toBraceClose();
    return toError();
}

void ArrayInitializer::stError() {
    throw exception::InvalidToken( *_it );
}


auto ArrayInitializer::beString() -> States {
    if ( _dimensions.size() != 1 )
        throw exception::InternalError( "cannot assign string field to multidimensional array" );
    if ( _baseType->size() != 1 )
        throw exception::InternalError( "cannot assign string field to not-char array" );
    if ( _dimensions.front() <= int( _it->value().size() ) )
        throw exception::InternalError( "cannot assign string field to short array" );

    for ( char c : _it->value() ) {
        _values.emplace_back( new ast::Constant( _it->position(), c, _parser.tree().typeStorage().fetchType( "char" ) ) );
    }
    for ( int i = _it->value().size(); i < _dimensions.back(); ++i ) {
        _values.emplace_back( new ast::Constant( _it->position(), 0, _parser.tree().typeStorage().fetchType( "char" ) ) );
    }

    return toQuit();
}


auto ArrayInitializer::toFirstBraceOpen() -> States {
    if ( _currentIndex.size() >= _dimensions.size() )
        throw exception::InternalError( "too much nesting braces" );

    _currentIndex.push_back( 0 );
    return States::BraceOpen;
}

auto ArrayInitializer::toNextBraceOpen() -> States {
    if ( _currentIndex.size() >= _dimensions.size() )
        throw exception::InternalError( "too much nesting braces" );
    if ( _currentIndex.empty() )
        throw exception::InternalError( "internal automaton error" );

    if ( _currentIndex.back() + 1 >= _dimensions[ _currentIndex.size() ] )
        throw exception::InternalError( "too much items in a row" );

    ++_currentIndex.back();
    return States::BraceOpen;
}

auto ArrayInitializer::toValue() -> States {
    if ( _currentIndex.size() != _dimensions.size() )
        throw exception::InternalError( "misplaced number" );

    if ( _currentIndex.back() >= _dimensions.back() )
        throw exception::InternalError( "too much items in a row" );

    ++_currentIndex.back();

    _values.emplace_back( Expression( _parser, _it ).obtain( true ) );
    _wait = true;

    return States::Value;
}

auto ArrayInitializer::toStringValue() -> States {
    if ( _currentIndex.size() + 1 != _dimensions.size() )
        throw exception::InternalError( "misplaced number" );

    switch ( _baseType->kind() ) {
    case ast::type::Kind::Elementary:
        if ( _baseType->size() != 1 )
            throw exception::InternalError( "cannot assign string field to not-char array" );
        if ( int( _it->value().size() ) >= _dimensions.back() )
            throw exception::InternalError( "too much items in a row" );

        for ( char c : _it->value() ) {
            _values.emplace_back( new ast::Constant( _it->position(), c, _parser.tree().typeStorage().fetchType( "char" ) ) );
        }

        for ( int i = _it->value().size(); i < _dimensions.back(); ++i ) {
            _values.emplace_back( new ast::Constant( _it->position(), 0, _parser.tree().typeStorage().fetchType( "char" ) ) );
        }

        break;
    case ast::type::Kind::Pointer:
        if ( _baseType->as< ast::type::Pointer >()->of().size() != 1 )
            throw exception::InternalError( "cannot assign string constant to pointer not to char" );

        _values.emplace_back( Expression( _parser, _it ).obtain( true ) );
        _wait = true;
        break;
    default:
        throw exception::InternalError( "cannot happen" );
    }


    return States::Value;
}

auto ArrayInitializer::toCommaInside() -> States {
    return States::CommaInside;
}

auto ArrayInitializer::toCommaOutside() -> States {
    return States::CommaOutside;
}

auto ArrayInitializer::toBraceClose() -> States {

    int lastIndex = _currentIndex.back();
    int neededIndexes = _dimensions[ _currentIndex.size() - 1 ];

    if ( lastIndex < neededIndexes ) {

        auto begin = _dimensions.begin() + _currentIndex.size();
        auto end = _dimensions.end();
        neededIndexes *= std::accumulate( begin, end, 1, []( int a, int b ) { return a * b; } );

        for ( ; lastIndex < neededIndexes; ++lastIndex )
            _values.emplace_back( new ast::Constant( _it->position(), 0, _baseType ) );
    }
    _currentIndex.pop_back();

    return
        _currentIndex.empty() ?
        toQuit() :
        States::BraceClose;
}

auto ArrayInitializer::toError() -> States {
    return States::Error;
}

auto ArrayInitializer::toQuit() -> States {
    _quit = true;
    return States::Quit;
}


ast::ArrayInitializer *ArrayInitializer::descend() {
    States states = States::Start;

    while ( _it && !_quit ) {
        switch ( states ) {
        case States::Start: states = stStart(); break;
        case States::BraceOpen: states = stBraceOpen(); break;
        case States::Value: states = stValue(); break;
        case States::CommaInside: states = stCommaInside(); break;
        case States::CommaOutside: states = stCommaOutside(); break;
        case States::BraceClose: states = stBraceClose(); break;
        case States::Quit: break;
        default:
        case States::Error: stError(); break;
        }

        if ( !_wait )
            ++_it; // move also after _quit == true
        _wait = false;
    }

    return new ast::ArrayInitializer( _begin->position(),
                                      _variable.release(),
                                      std::move( _dimensions ),
                                      std::move( _values ) );
}

} // namespace parser
} // namespace compiler
