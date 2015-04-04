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
    Token result( Type::Word, items.back().value() );
    result.value() += toJoin.front().value();
    items.back() = result;

    if ( toJoin.size() > 1 )
        merge( items, Substituer( *this, toJoin.begin() + 1, toJoin.end() ).result() );
}

void Substituer::merge( std::vector< Token > &destination, std::vector< Token > &source ) {
    std::move( source.begin(), source.end(), std::back_inserter( destination ) );
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

std::vector< Token > Substituer::substitute( int *consumed ) {

    Token token = _chunks.top();
    _chunks.pop();

    if ( token.type() != Type::Word ) {
        if ( consumed )
            *consumed = 1;
        return{ token };
    }

    auto symbol = _symbols.find( token.value() );

    if ( !symbol ) {
        if ( consumed )
            *consumed = 1;
        return{ token };
    }

    if ( symbol->kind() == Symbol::Kind::Function ) {
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

        std::vector< Token > items;
        for ( const auto &chunk : symbol->value() ) {

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

            ptrdiff_t index = grabParameterIndex( *symbol, chunk );
            if ( index < 0 ) {
                addChunk( items, chunk );
                continue;
            }

            if ( _stringify )
                stringify( items, actualParams[ index ] );
            else if ( _join )
                join( items, actualParams[ index ] );
            else
                merge( items, Substituer( *this, actualParams[ index ].begin(), actualParams[ index ].end() ).result() );
        }
        return recursion( UsedSymbol( token, actualParams ), std::move( items ) );
    }
    if ( consumed )
        *consumed = 1;
    return recursion( UsedSymbol( token ), symbol->value() );
}

} // namespace preprocessor
} // namespace compiler
