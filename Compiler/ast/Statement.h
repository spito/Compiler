#pragma once

#include "Core.h"
#include "Traversal.h"
#include "../common/Position.h"

#include <memory>
#include <type_traits>

namespace compiler {
namespace ast {

template< typename SI >
struct StoredInfo {
    SI &storedInfo() {
        return _info;
    }

    const SI &storedInfo() const {
        return _info;
    }
private:
    SI _info;
};

template<>
struct StoredInfo < void > {};

template< typename Traversal >
struct Statement : DynamicCast, StoredInfo< typename Traversal::StoredInfo > {

    using Handle = std::unique_ptr< Statement >;
    using EHandle = std::unique_ptr< Expression< Traversal > >;
    using Ptr = Statement *;
    using EPtr = Expression< Traversal > *;
    using ConstPtr = const Statement *;
    using ConstEPtr = const Expression< Traversal > *;
    using Information = typename Traversal::Information;

    Statement( common::Position p = common::Position() ) :
        _position( std::move( p ) )
    {}

    virtual ~Statement() = default;

    virtual Information *traverse( Traversal & ) const = 0;

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

    common::Position &position() {
        return _position;
    }

    const common::Position &position() const {
        return _position;
    }
private:
    Ptr _parentBreak = nullptr;
    Ptr _parentContinue = nullptr;
    common::Position _position;
};

} // namespace ast
} // namespace compiler
