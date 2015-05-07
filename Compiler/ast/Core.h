#pragma once

namespace compiler {
namespace ast {

struct Expression;
struct UnaryOperator;
struct BinaryOperator;
struct TernaryOperator;
struct Call;
struct Variable;
struct Constant;
struct ArrayInitializer;
struct StringPlaceholder;
struct Block;
struct If;
struct DoWhile;
struct While;
struct For;
//struct StatementSwitch;
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
    ArrayInitializer,
    StringPlaceholder,
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

