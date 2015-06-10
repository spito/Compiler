#pragma once

#include "Operand.h"
#include "../includes/exceptions.h"

#include <string>
#include <vector>

namespace compiler {
namespace code {

enum class InstructionName {
    Global,
    Alloc,
    Load,
    Store,

    IndexAt,

    Multiplication,
    Division,
    Modulo,

    Addition,
    Subtraction,

    BitwiseShiftLeft,
    BitwiseShiftRight,
    BitwiseAnd,
    BitwiseXor,
    BitwiseOr,

    ExtenseSigned,
    ExtenseUnsigned,
    Reduce,
    BitCast,
    PtrToInt,

    CompareEqual,
    CompareNotEqual,
    CompareLess,
    CompareLessEqual,
    CompareGreater,
    CompareGraterEqual,

    Jump,
    Branch,
    Merge,

    Call,
    Return,
    //Label,
};

struct Instruction {

    Instruction( InstructionName name, std::vector< Operand > o ) :
        _name( name ),
        _operands( std::move( o ) )
    {}

    Instruction( const Instruction & ) = default;
    Instruction( Instruction &&other ) :
        _name( other._name ),
        _operands( std::move( other._operands ) )
    {}

    Instruction &operator=( Instruction other ) {
        swap( other );
        return *this;
    }

    void swap( Instruction &other ) {
        using std::swap;

        swap( _name, other._name );
        swap( _operands, other._operands );
    }

    InstructionName name() const {
        return _name;
    }

    const std::vector< Operand > &operands() const {
        return _operands;
    }

    size_t size() const {
        return _operands.size();
    }

    const Operand &operand( size_t index ) const {
        if ( index >= size() )
            throw exception::InternalError( "index out of range" );
        return _operands[ index ];
    }

private:
    InstructionName _name;
    std::vector< Operand > _operands;

};

} // namespace code
} // namespace compiler

namespace std {

template<>
inline void swap< compiler::code::Instruction >( compiler::code::Instruction &lhs, compiler::code::Instruction &rhs ) {
    lhs.swap( rhs );
}

} // namespace std
