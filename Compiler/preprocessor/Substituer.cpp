#include "Substituer.h"

namespace compiler {
namespace preprocessor {

static ptrdiff_t grabParameterIndex( const Symbol &symbol, const std::string &name ) {
    ptrdiff_t index = 0;
    for ( const auto &param : symbol.parametres() ) {
        if ( param == name )
            return index;
        ++index;
    }
    return -1;
}

void Substituer::prepareForJoin( std::vector< std::string > &items ) {
    while ( !items.empty() ) {
        if ( common::isSpace( items.back().c_str() ) )
            items.pop_back();
        else if ( common::isWord( items.back().c_str() ) )
            break;
        else
            throw exception::InternalError( "malformed source - word has to be before ##" );
    }
    if ( items.empty() )
        throw exception::InternalError( "malformed source - word has to be before ##" );
}

void Substituer::addChunk( std::vector< std::string > &items, const std::string &chunk ){
    if ( _stringify && !common::isSpace( chunk.c_str() ) )
        throw exception::InvalidCharacterConstant( "#" );
    else if ( _join ) {
        if ( common::isSpace( chunk.c_str() ) )
            return;
        items.back() += chunk;
        _join = false;
    }
    else
        items.push_back( chunk );
}

void Substituer::stringify( std::vector< std::string > &items, const std::vector< std::string > &toString ) {
    items.push_back( "\"" );
    for ( const auto &s : toString )
        items.back() += s;
    items.back() += "\"";
    _stringify = false;
}

void Substituer::join( std::vector< std::string > &items, const std::vector< std::string > &toJoin ){
    _join = false;
    items.back() += toJoin.front();
    items.insert( items.end(), toJoin.begin() + 1, toJoin.end() );
}

void Substituer::recursion( UsedSymbol &&symbol, const std::vector< std::string > &items, int toPop ) {
    if ( _used.find( symbol ) ) {
        _result += symbol.name();
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

    std::string value = _chunks->top();
    _chunks->pop();

    auto symbol = _symbols->find( value );

    if ( !symbol ) {
        _result += value;
        return 1;
    }

    if ( symbol->kind() == Symbol::Kind::Function ) {
        Parametrizer parametrizer( _chunks->begin() );
        if ( parametrizer.ignored() ) {
            _result += value;
            return 1;
        }
        auto actualParams = parametrizer.result();
        std::vector< std::string > items;
        for ( const auto &chunk : symbol->value() ) {

            if ( chunk == "#" && !_stringify ) {
                _stringify = true;
                continue;
            }
            else if ( chunk == "##" ) {
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
        recursion( UsedSymbol( value, actualParams ), items, parametrizer.consumed() );
        return 1 + consumed;// symbol + ( parametres, ... )
    }
    else {
        recursion( UsedSymbol( value ), symbol->value() );
        return 1;
    }
}


} // namespace preprocessor
} // namespace compiler
