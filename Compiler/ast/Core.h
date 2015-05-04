#pragma once

namespace compiler {
namespace ast {

//template< typename Traversal > struct Expression;
//template< typename Traversal > struct UnaryOperator;
//template< typename Traversal > struct BinaryOperator;
//template< typename Traversal > struct TernaryOperator;
//template< typename Traversal > struct Call;
//template< typename Traversal > struct Variable;
//template< typename Traversal > struct Constant;
//template< typename Traversal > struct Block;
//template< typename Traversal > struct If;
//template< typename Traversal > struct DoWhile;
//template< typename Traversal > struct While;
//template< typename Traversal > struct For;
//template< typename Traversal > struct StatementSwitch;
////struct StatementEcho;
//template< typename Traversal > struct Break;
//template< typename Traversal > struct Continue;
//template< typename Traversal > struct Return;

struct Expression;
struct UnaryOperator;
struct BinaryOperator;
struct TernaryOperator;
struct Call;
struct Variable;
struct Constant;
struct Block;
struct If;
struct DoWhile;
struct While;
struct For;
struct StatementSwitch;
//struct StatementEcho;
struct Break;
struct Continue;
struct Return;

enum class Kind {
    UnaryOperator,
    BinaryOperator,
    TernaryOperator,
    Call,
    Variable,
    Constant,
    Block,
    If,
    DoWhile,
    While,
    For,
    Switch,
    Break,
    Continue,
    Return,
};

struct DynamicCast {

    virtual ~DynamicCast() = default;

    template< typename T >
    T *as() {
        return dynamic_cast<T *>( this );
    }
    template< typename T >
    const T *as() const {
        return dynamic_cast< const T *>( this );
    }
};

} // namespace ast
} // namespace compiler

