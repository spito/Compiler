#pragma once

#include "Core.h"
#include "../common/Position.h"

#include <memory>
#include <type_traits>

namespace compiler {
namespace ast {

struct Statement : DynamicCast {

    using Handle = std::unique_ptr< Statement >;
    using EHandle = std::unique_ptr< Expression >;
    using Ptr = Statement *;
    using EPtr = Expression *;
    using ConstPtr = const Statement *;
    using ConstEPtr = const Expression *;
    using Base = Statement;

    Statement( Kind k, common::Position p = common::Position() ) :
        _kind( k ),
        _position( std::move( p ) )
    {}

    Statement( const Statement &o ) :
        _kind( o._kind ),
        _position( o._position )
    {}

    Statement( Statement &&o ) :
        _kind( o._kind ),
        _position( std::move( o._position ) )
    {}

    virtual ~Statement() = default;

    Kind kind() const {
        return _kind;
    }

    common::Position &position() {
        return _position;
    }

    const common::Position &position() const {
        return _position;
    }
protected:
    void import( Statement &other ) {
        using std::swap;

        swap( _kind, other._kind );
        swap( _position, other._position );
    }
private:
    Kind _kind;
    common::Position _position;
};

struct Cycle : Statement {

    using Base = Cycle;

    Cycle( Kind k, common::Position p = common::Position() ) :
        Statement( k, std::move( p ) )
    {}

    Cycle( const Cycle &o ) :
        Statement( o )
    {}

    Cycle( Cycle &&o ) :
        Statement( std::move( o ) )
    {}

};

} // namespace ast
} // namespace compiler
