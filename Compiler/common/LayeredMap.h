#pragma once

#include <map>
#include <list>

namespace compiler {
namespace common {

template< typename Value >
struct Carriage {
    Carriage( Value v, int i ) :
        value( std::move( v ) ),
        index( i )
    {}

    Value value;
    int index;
};


template<
    typename Key,
    typename Value,
    typename Compare = std::less< Key >,
    typename Allocator = std::allocator< std::pair< const Key, std::list< Carriage< Value > > > > >
struct LayeredMap {

    using key_type = Key;
    using mapped_type = Value;
    //using value_type = 
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using key_compare = Compare;
    using allocator_type = Allocator;
    //using reference = value_t
    //using 

    LayeredMap() :
        _layerIndex( 0 )
    {}

    LayeredMap( const LayeredMap &other ) = default;
    //LayeredMap( LayeredMap && ) = default;
    LayeredMap( LayeredMap &&other ) :
        _layerIndex( other._layerIndex ),
        _data( std::move( other._data ) )
    {}

    LayeredMap &operator=( LayeredMap other ) {
        swap( other );
        return *this;
    }

    Value &operator[]( const Key &key ) {
        auto i = _data.find( key );
        if ( i == _data.end() )
            i = _data.insert( key, std::list< Carriage< Value > >() ).first;

        if ( i->second.empty() )
            i->second.emplace_back();

        return i->second.front().value;
    }

    void insert( Key &&key, Value &&value ) {
        auto i = _data.find( key );
        if ( i == _data.end() )
            i = _data.emplace( std::move( key ), std::list< Carriage< Value > >() ).first;

        i->second.emplace_front( std::move( value ), _layerIndex );
    }

    void insert( const Key &key, const Value &value ) {
        auto i = _data.find( key );
        if ( i == _data.end() )
            i = _data.insert( key, std::list< Carriage< Value > >() ).first;

        i->emplace_front( value, _layerIndex );
    }

    const Value *find( const Key &key ) const {
        auto i = _data.find( key );
        if ( i == _data.end() )
            return nullptr;

        if ( i->second.empty() )
            return nullptr;

        return &i->second.front().value;
    }

    int count( const Key &key ) const {
        return find( key ) ? 1 : 0;
    }

    void addLayer() {
        ++_layerIndex;
    }

    void removeLayer() {
        --_layerIndex;

        for ( auto &i : _data ) {
            while ( !i.second.empty() && i.second.front().index > _layerIndex )
                i.second.pop_front();
        }
    }

private:


    int _layerIndex;

    std::map< Key, std::list< Carriage< Value > > > _data;

};


} // namespace common
} // namespace compiler

