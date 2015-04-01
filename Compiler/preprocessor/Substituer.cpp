#include "Substituer.h"

namespace compiler {
namespace preprocessor {

static ptrdiff_t grabParameterIndex( const Symbol &symbol, const common::Token &name ) {
    ptrdiff_t index = 0;
    for ( const auto &param : symbol.parametres() ) {
        if ( param == name )
            return index;
        ++index;
    }
    return -1;
}

void Substituer::prepareForJoin( std::vector< common::Token > &items ) {
    while ( !items.empty() ) {
        if ( items.back().type() == common::Token::Type::Space )
            items.pop_back();
        else if ( items.back().type() == common::Token::Type::Word )
            break;
        else
            throw exception::InternalError( "malformed source - word has to be before ##" );
    }
    if ( items.empty() )
        throw exception::InternalError( "malformed source - word has to be before ##" );
}

void Substituer::addChunk( std::vector< common::Token > &items, const common::Token &chunk ){
    if ( _stringify && chunk.type() != common::Token::Type::Space )
        throw exception::InvalidCharacterConstant( "#" );
    else if ( _join ) {
        if ( chunk.type() != common::Token::Type::Space )
            return;
        items.back().value() += chunk.value();
        _join = false;
    }
    else
        items.push_back( chunk );
}

void Substituer::stringify( std::vector< common::Token > &items, const std::vector< common::Token > &toString ) {
    common::Token result( common::Token::Type::String );

    for ( const auto &s : toString )
        result.value() += s.value();

    items.push_back( std::move( result ) );
    _stringify = false;
}

void Substituer::join( std::vector< common::Token > &items, const std::vector< common::Token > &toJoin ){
    _join = false;
    common::Token result( common::Token::Type::Word, items.back().value() );
    result.value() += toJoin.front().value();
    items.back() = result;
    items.insert( items.end(), toJoin.begin() + 1, toJoin.end() );
}

void Substituer::recursion( UsedSymbol &&symbol, const std::vector< common::Token > &items, int toPop ) {
    if ( _used.find( symbol ) ) {
        _result.push_back( symbol.token() );
        return;
    }
    _used.insert( UsedSymbol( symbol ) );
    if ( toPop )
        _chunks->pop( toPop );
    _chunks->prepend( items );
    for ( int eaten = 0; eaten < int( items.size() ); )
        eaten += substitute();
    _used.remove( symbol );
}

int Substituer::substitute() {

    common::Token token = _chunks->top();
    _chunks->pop();

    auto symbol = _symbols->find( token.value() );

    if ( !symbol ) {
        _result.push_back( std::move( token ) );
        return 1;
    }

    if ( symbol->kind() == Symbol::Kind::Function ) {
        Parametrizer parametrizer( _chunks->begin() );
        if ( parametrizer.ignored() ) {
            _result.push_back( std::move( token ) );
            return 1;
        }
        auto actualParams = parametrizer.result();
        std::vector< common::Token > items;
        for ( const auto &chunk : symbol->value() ) {

            if ( chunk.value() == "#" && !_stringify ) {
                _stringify = true;
                continue;
            }
            else if ( chunk.value() == "##" ) {
                _join = true;
                prepareForJoin( items );
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
                items.insert( items.end(), actualParams[ index ].begin(), actualParams[ index ].end() );
        }
        int consumed = parametrizer.consumed();
        recursion( UsedSymbol( token, actualParams ), items, parametrizer.consumed() );
        return 1 + consumed;// symbol + ( parametres, ... )
    }
    else {
        recursion( UsedSymbol( token ), symbol->value() );
        return 1;
    }
}


} // namespace preprocessor
} // namespace compiler
