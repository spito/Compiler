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

    AddressOf,
    Dereference,
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

    Extense,
    Reduce,
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
    Label,
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

    InstructionName name() const {
        return _name;
    }

    const std::vector< Operand > &operands() const {
        return _operands;
    }

private:
    InstructionName _name;
    std::vector< Operand > _operands;

};


} // namespace code
} // namespace compiler
