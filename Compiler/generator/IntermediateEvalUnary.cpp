#include "Intermediate.h"

namespace compiler {
namespace generator {

auto Intermediate::opLogicalNot( Operand input ) -> Operand {

    Operand partial( newRegister( code::Type( 1, false ) ) );

    addInstruction( code::InstructionName::CompareEqual, {
        partial,
        input,
        Operand( 0, input.type() )
    } );

    Operand result( newRegister( input.type() ) );
    addInstruction( code::InstructionName::Extense, {
        result, 
        partial
    } );

    return result;
}

auto Intermediate::opAddressOf( Operand input ) -> Operand {
    return input;
}

auto Intermediate::opDereference( Operand input ) -> Operand {

    code::Type type( input.type() );
    type.removeIndirection();

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        result,
        input
    } );
    return result;
}

auto Intermediate::opPrefixDecrement( Operand input ) -> Operand {

    code::Type type( input.type() );
    type.removeIndirection();
    Operand partial( newRegister( type ) );

    addInstruction( code::InstructionName::Load, {
        partial,
        input
    } );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Subtraction, {
        result,
        partial,
        Operand( 1, type )
    } );

    addInstruction( code::InstructionName::Store, {
        result,
        input
    } );

    return result;
}

auto Intermediate::opPrefixIncrement( Operand input ) -> Operand {

    code::Type type( input.type() );
    type.removeIndirection();
    Operand partial( newRegister( type ) );

    addInstruction( code::InstructionName::Load, {
        partial,
        input
    } );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Addition, {
        result,
        partial,
        Operand( 1, type )
    } );

    addInstruction( code::InstructionName::Store, {
        result,
        input
    } );

    return result;

}

auto Intermediate::opSuffixDecrement( Operand input ) -> Operand {
    code::Type type( input.type() );
    type.removeIndirection();

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        result,
        input
    } );

    Operand partial( newRegister( type ) );
    addInstruction( code::InstructionName::Subtraction, {
        partial,
        result,
        Operand( 1, type )
    } );

    addInstruction( code::InstructionName::Store, {
        partial,
        input
    } );

    return result;
}

auto Intermediate::opSuffixIncrement( Operand input ) -> Operand {
    code::Type type( input.type() );
    type.removeIndirection();

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        result,
        input
    } );

    Operand partial( newRegister( type ) );
    addInstruction( code::InstructionName::Addition, {
        partial,
        result,
        Operand( 1, type )
    } );

    addInstruction( code::InstructionName::Store, {
        partial,
        input
    } );

    return result;
}

auto Intermediate::opBitwiseNot( Operand input ) -> Operand {
    code::Type type( input.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseXor, {
        result,
        input,
        Operand( -1, type )
    } );
    return result;
}

auto Intermediate::opUnaryMinus( Operand input ) -> Operand {
    code::Type type( input.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Subtraction, {
        result,
        Operand( 0, type ),
        input
    } );
    return result;
}


} // namespace generator
} // namespace compiler
