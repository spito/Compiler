#include "Substituer.h"

namespace compiler {
namespace preprocessor {

using Token = common::Token;
using Type = common::Token::Type;
using Operator = common::Operator;

static ptrdiff_t grabParameterIndex( const Symbol &symbol, const Token &name ) {
    ptrdiff_t index = 0;
    for ( const auto &param : symbol.parametres() ) {
        if ( param == name )
            return index;
        ++index;
    }
    return -1;
}

static void merge( std::vector< Token > &destination, std::vector< Token > &source ) {
    if ( !source.empty() )
        std::move( source.begin(), source.end(), std::back_inserter( destination ) );
}
static void merge( std::vector< Token > &destination, std::vector< Token > &&source ) {
    merge( destination, source );
}


void Substituer::addChunk( std::vector< Token > &items, const Token &chunk ){
    if ( _stringify )
        throw exception::InternalError( "malformed source - parameter has to be after #" );
    else if ( _join ) {
        items.back().value() += chunk.value();
        _join = false;
    }
    else
        items.push_back( chunk );
}

void Substituer::stringify( std::vector< Token > &items, const std::vector< Token > &toString ) {
    Token result( Type::String );

    for ( const auto &s : toString )
        result.value() += s.value();

    items.push_back( std::move( result ) );
    _stringify = false;
}

void Substituer::join( std::vector< Token > &items, const std::vector< Token > &toJoin ){
    _join = false;
    Token result( items.back().value() );
    result.value() += toJoin.front().value();
    items.back() = result;

    if ( toJoin.size() > 1 )
        merge( items, Substituer( *this, toJoin.begin() + 1, toJoin.end() ).result() );
}

std::vector< Token > Substituer::recursion( UsedSymbol &&symbol, std::vector< Token > items ) {
    if ( _used.find( symbol ) )
        return{ symbol.token() };

    _used.insert( UsedSymbol( symbol ) );

    _chunks.prepend( items );
    std::vector< Token > result;

    for ( int eaten = 0; eaten < int( items.size() ); ) {
        int consumed;
        merge( result, substitute( &consumed ) );
        eaten += consumed;
    }
    _used.remove( symbol );
    return result;
}

void Substituer::recursion( std::vector< common::Token >::const_iterator begin, std::vector< common::Token >::const_iterator end ) {
    _chunks.prepend( begin, end );
    auto diff = end - begin;
    for ( int eaten = 0; eaten < diff; ) {
        int consumed;
        merge( _result, substitute( &consumed ) );
        eaten += consumed;
    }
}

std::vector< Token > Substituer::substitute( int *consumed ) {

    Token token = _chunks.top();
    _chunks.pop();

    if ( token.type() != Type::Word ) {
        if ( consumed )
            *consumed = 1;
        return{ token };
    }

    auto symbol = _symbols.find( token.value() );

    if ( !symbol || ( !_insideExpression && symbol->kind() == Symbol::Kind::Defined ) ) {
        if ( consumed )
            *consumed = 1;
        if ( _insideExpression ) {
            Token t( "0", Type::Integer, token.position() );
            t.integer() = 0;
            return{ t };
        }
        return{ token };
    }

    switch ( symbol->kind() ) {
    case Symbol::Kind::Integer:
        return substituteInteger( std::move( token ), *symbol, consumed );
    case Symbol::Kind::Macro:
        return substituteMacro( std::move( token ), *symbol, consumed );
    case Symbol::Kind::Special:
        return substituteSpecial( token, *symbol, consumed );
    case Symbol::Kind::Defined:
    case Symbol::Kind::Function:
        return substituteFunction( std::move( token ), *symbol, consumed );
    case Symbol::Kind::Nothing:
        if ( consumed )
            *consumed = 1;
        return{};
    default:
        throw exception::InternalError( "unknown kind of symbol" );
    }
}

std::vector< Token > Substituer::substituteMacro( Token token, const Symbol &symbol, int *consumed ) {
    if ( consumed )
        *consumed = 1;
    auto tokens = symbol.value();
    for ( auto &t : tokens )
        t.position() = token.position();
    return recursion( UsedSymbol( std::move( token ) ), std::move( tokens ) );
}

std::vector< Token > Substituer::substituteInteger( Token token, const Symbol &symbol, int *consumed ) {
    if ( consumed )
        *consumed = 1;
    token.replaceBy( symbol.value().front() );
    return{ token };
}

std::vector< Token > Substituer::substituteFunction( Token token, const Symbol &symbol, int *consumed ) {
    std::vector< std::vector< Token > > actualParams;
    savePosition();
    Parametrizer parametrizer( _chunks.begin(), limited() );
    if ( parametrizer.ignored() ) {
        restorePosition();
        if ( consumed )
            *consumed = 1;
        return{ token };
    }

    actualParams = parametrizer.result();
    _chunks.pop( parametrizer.consumed() );
    if ( consumed )
        *consumed = 1 + parametrizer.consumed();

    if ( actualParams.size() != symbol.parametres().size() )
        throw exception::InternalError( "malformed source - insufficient number of parametres passed into the function macro" );

    if ( symbol.kind() == Symbol::Kind::Defined ) {
        if ( actualParams.front().size() != 1 )
            throw exception::InternalError( "only one macro name can be passed into defined operator" );

        Token t( "0", Type::Integer );
        t.integer() = 0;
        if ( _symbols.find( actualParams.front().front().value() ) ) {
            t.value() = "1";
            t.integer() = 1;
        }
        return{ t };
    }

    std::vector< Token > items;
    for ( const auto &chunk : symbol.value() ) {

        if ( chunk.isOperator( Operator::Sharp ) ) {
            if ( _join )
                throw exception::InternalError( "malformed source - # cannot follow ##" );
            _stringify = true;
            continue;
        }
        if ( chunk.isOperator( Operator::TwoShaprs ) ) {
            _join = true;
            if ( _stringify )
                throw exception::InternalError( "malformed source - ## cannot follow #" );
            if ( items.empty() || items.back().type() != common::Token::Type::Word )
                throw exception::InternalError( "malformed source - word has to be before ##" );
            continue;
        }

        ptrdiff_t index = grabParameterIndex( symbol, chunk );
        if ( index < 0 ) {
            addChunk( items, chunk );
            continue;
        }
        else if ( _stringify )
            stringify( items, actualParams[ index ] );
        else if ( _join )
            join( items, actualParams[ index ] );
        else
            merge( items, Substituer( *this, actualParams[ index ].begin(), actualParams[ index ].end() ).result() );
    }
    return recursion( UsedSymbol( token, actualParams ), std::move( items ) );
}

std::vector< Token > Substituer::substituteSpecial( const Token &token, const Symbol &symbol, int *consumed ) {
    if ( consumed )
        *consumed = 1;

    if ( symbol.name() == "__FILE__" )
        return{ Token( token.position().file(), Type::String, token.position() ) };
    if ( symbol.name() == "__LINE__" ) {
        Token t( std::to_string( token.position().line() ), Type::Integer, token.position() );
        t.integer() = token.position().line();
        return{ t };
    }
    return{};
}


} // namespace preprocessor
} // namespace compiler
