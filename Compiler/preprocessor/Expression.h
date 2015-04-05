#pragma once

#include "../common/Token.h"
#include "../includes/exceptions.h"

#include <memory>
#include <vector>

namespace compiler {
namespace preprocessor {

struct IteratorPack {
    using Iterator = std::vector< common::Token >::const_iterator;

    IteratorPack( Iterator b, Iterator e ) :
        begin( b ),
        end( e )
    {}

    Iterator begin;
    Iterator end;

    bool valid() const {
        return begin != end;
    }
    explicit operator bool() const {
        return valid();
    }

    IteratorPack &operator++( ) {
        ++begin;
        return *this;
    }

    const common::Token *operator->( ) const {
        return begin.operator->();
    }
    const common::Token &operator*( ) const {
        return *begin;
    }

};

struct Expression {

    using Value = long long;
    using Unsigned = unsigned long long;
    using Handle = std::unique_ptr < Expression > ;
    using Iterator = std::vector< common::Token >::iterator;

    enum class Side : bool {
        Left,
        Right
    };

    static void prepare( Iterator, Iterator );

    Expression( common::Operator owner = common::Operator::None ) :
        _value( 0 ),
        _op( common::Operator::None ),
        _owner( owner )
    {}
    Expression( IteratorPack &&pack ) :
        Expression( pack )
    {
        if ( pack )
            throw exception::InvalidToken( *pack );
    }
    Expression( const Expression & ) = delete;
    Expression( Expression &&other ) :
        _value( other._value ),
        _op( other._op ),
        _owner( other._owner ),
        _lhs( std::move( other._lhs ) ),
        _rhs( std::move( other._rhs ) )
    {}

    Expression &operator=( Expression &&other ) {
        _value = other._value;
        _op = other._op;
        _owner = other._owner;
        _lhs.swap( other._lhs );
        _rhs.swap( other._rhs );
        return *this;
    }

    Value value();

    explicit operator bool() {
        return value() != 0;
    }

private:

    Expression( IteratorPack &, Side = Side::Left, common::Operator = common::Operator::None );

    Expression &left() {
        if ( !_lhs ) {
            if ( isPrefix() )
                throw exception::InternalError( "attempt to remove part of expression" );
            _lhs.reset( new Expression() );
        }
        return *_lhs;
    }
    Expression &right() {
        if ( !_rhs ) {
            if ( isPostfix() )
                throw exception::InternalError( "attempt to remove part of expression" );
            _rhs.reset( new Expression() );
        }
        return *_rhs;
    }

    bool isCloser( common::Operator ) const;
    bool isFarther( common::Operator ) const;
    bool isUnary() const;
    bool isPrefix() const;
    bool isPostfix() const;
    bool isBinary() const;

    Value _value;
    common::Operator _op;
    common::Operator _owner;
    Handle _lhs;
    Handle _rhs;
};


} // namespace preprocessor
} // namespace compiler

