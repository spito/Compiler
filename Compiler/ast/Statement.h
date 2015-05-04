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

    Statement( Kind k, common::Position p = common::Position() ) :
        _kind( k ),
        _position( std::move( p ) )
    {}

    Statement( const Statement &o ) :
        _kind( o._kind ),
        _position( o._position ),
        _parentBreak( o._parentBreak ),
        _parentContinue( o._parentContinue )
    {}

    Statement( Statement &&o ) :
        _kind( o._kind ),
        _position( std::move( o._position ) ),
        _parentBreak( o._parentBreak ),
        _parentContinue( o._parentContinue )
    {}

    virtual ~Statement() = default;

    Ptr parentBreak() const {
        return _parentBreak;
    }
    virtual void parentBreak( Ptr p ) {
        if ( !_parentBreak )
            _parentBreak = p;
    }

    Ptr parentContinue() const {
        return _parentContinue;
    }
    virtual void parentContinue( Ptr p ) {
        if ( !_parentContinue )
            _parentContinue = p;
    }

    Kind kind() const {
        return _kind;
    }

    common::Position &position() {
        return _position;
    }

    const common::Position &position() const {
        return _position;
    }
private:
    Kind _kind;
    Ptr _parentBreak = nullptr;
    Ptr _parentContinue = nullptr;
    common::Position _position;
};

} // namespace ast
} // namespace compiler
