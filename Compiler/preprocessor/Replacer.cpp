#include "Replacer.h"

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


void Replacer::addChunk( std::vector< Token > &items, const Token &chunk ){
    if ( _stringify )
        throw exception::InternalError( "malformed source - parameter has to be after #" );
    else if ( _join ) {
        items.back().value() += chunk.value();
        _join = false;
    }
    else
        items.push_back( chunk );
}

void Replacer::stringify( std::vector< Token > &items, const std::vector< Token > &toString ) {
    Token result( Type::String );

    for ( const auto &s : toString )
        result.value() += s.value();

    items.push_back( std::move( result ) );
    _stringify = false;
}

void Replacer::join( std::vector< Token > &items, const std::vector< Token > &toJoin ){
    _join = false;
    Token result( items.back().value() );
    result.value() += toJoin.front().value();
    items.back() = result;

    if ( toJoin.size() > 1 )
        merge( items, Replacer( *this, toJoin.begin() + 1, toJoin.end() ).result() );
}

std::vector< Token > Replacer::recursion( UsedSymbol &&symbol, std::vector< Token > items ) {
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

void Replacer::recursion( std::vector< common::Token >::const_iterator begin, std::vector< common::Token >::const_iterator end ) {
    _chunks.prepend( begin, end );
    auto diff = end - begin;
    for ( int eaten = 0; eaten < diff; ) {
        int consumed;
        merge( _result, substitute( &consumed ) );
        eaten += consumed;
    }
}

std::vector< Token > Replacer::substitute( int *consumed ) {

    Token token = _chunks.top();
    _chunks.pop();

    if ( token.type() != Type::Word ) {
        if ( consumed )
            *consumed = 1;
        return{ token };
    }

    auto symbol = _symbols.find( token.value() );

    if ( !symbol || ( !_insideExpression && symbol->expressionOnly() ) ) {
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

std::vector< Token > Replacer::substituteMacro( Token token, const Symbol &symbol, int *consumed ) {
    if ( consumed )
        *consumed = 1;

    if ( symbol.special() )
        return symbol.eval( token );

    auto tokens = symbol.value();
    for ( auto &t : tokens )
        t.position() = token.position();
    return recursion( UsedSymbol( std::move( token ) ), std::move( tokens ) );
}

std::vector< Token > Replacer::substituteInteger( Token token, const Symbol &symbol, int *consumed ) {
    if ( consumed )
        *consumed = 1;
    token.replaceBy( symbol.value().front() );
    return{ token };
}

std::vector< Token > Replacer::substituteFunction( Token token, const Symbol &symbol, int *consumed ) {
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

    if ( symbol.special() )
        return symbol.eval( token, &actualParams );

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
            merge( items, Replacer( *this, actualParams[ index ].begin(), actualParams[ index ].end() ).result() );
    }
    for ( auto &t : items )
        t.position() = token.position();
    return recursion( UsedSymbol( token, actualParams ), std::move( items ) );
}

} // namespace preprocessor
} // namespace compiler
