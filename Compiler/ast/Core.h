#pragma once

namespace compiler {
namespace ast {

template< typename Traversal > struct Expression;
template< typename Traversal > struct UnaryOperator;
template< typename Traversal > struct BinaryOperator;
template< typename Traversal > struct TernaryOperator;
template< typename Traversal > struct Call;
template< typename Traversal > struct Variable;
template< typename Traversal > struct Constant;
template< typename Traversal > struct Block;
template< typename Traversal > struct If;
template< typename Traversal > struct DoWhile;
template< typename Traversal > struct While;
template< typename Traversal > struct For;
template< typename Traversal > struct StatementSwitch;
//struct StatementEcho;
template< typename Traversal > struct Break;
template< typename Traversal > struct Continue;
template< typename Traversal > struct Return;

struct DynamicCast {

    virtual ~DynamicCast() = default;

    template< typename T >
    T *as() {
        return dynamic_cast<T *>( this );
    }
    template< typename T >
    const T *as() const {
        return dynamic_cast<T *>( this );
    }
};

} // namespace ast
} // namespace compiler

