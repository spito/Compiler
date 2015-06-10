#include "Intermediate.h"

namespace compiler {
namespace generator {

auto Intermediate::opLogicalOr( const ast::BinaryOperator *e, Access access ) -> Operand {

    int original = _currentBlock;
    int secondPart = addBasicBlock();
    int next = addBasicBlock();

    block( secondPart ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( secondPart );

    Operand partial = eval( e->left(), access );

    addBranch( partial, next, secondPart );

    refreshBlock( secondPart );
    Operand full = eval( e->right(), access );
    addJump( next );

    refreshBlock( next );
    Operand result = newRegister( code::Type( 1, false ) );
    addInstruction( code::InstructionName::Merge, {
        result,
        Operand::label( block( original ).id() ),
        partial,
        Operand::label( block( secondPart ).id() ),
        full
    } );
    return result;
}

auto Intermediate::opLogicalAnd( const ast::BinaryOperator *e, Access access ) -> Operand {
    int original = _currentBlock;
    int secondPart = addBasicBlock();
    int next = addBasicBlock();

    block( secondPart ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( secondPart );

    Operand partial = eval( e->left(), access );

    addBranch( partial, secondPart, next );

    refreshBlock( secondPart );
    Operand full = eval( e->right(), access );
    addJump( next );

    refreshBlock( next );
    Operand result = newRegister( code::Type( 1, false ) );
    addInstruction( code::InstructionName::Merge, {
        result,
        Operand::label( block( original ).id() ),
        partial,
        Operand::label( block( secondPart ).id() ),
        full
    } );
    return result;


}


auto Intermediate::opArrayAccess( Operand left, Operand right, Access access ) -> Operand {

    code::Type type( left.type() );
    type.removeIndirection();

    Operand address( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        address,
        left
    } );

    Operand pointer( newRegister( type ) );
    addInstruction( code::InstructionName::IndexAt, {
        pointer,
        address,
        right
    } );

    if ( access == Access::Store )
        return pointer;

    type.removeIndirection();
    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        result,
        pointer
    } );

    return result;
}

auto Intermediate::opTypeCast( code::Type type, Operand input ) -> Operand {
#pragma message("warning: implement typecast")
    return input;
#if 0
    if ( input.type().indirection() == 0 ) {
        if ( type. )
    }
    else {
        if ( type.indirection() == 0 ) {
            Operand result( newRegister( type ) );
            addInstruction( code::InstructionName::PtrToInt, {
                result,
                input
            } );
            return result;
        }
    }
#endif
}

auto Intermediate::opMultiplication( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Multiplication, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opDivision( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Division, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opModulo( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Modulo, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opAddition( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Addition, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opSubtraction( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Subtraction, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseLeftShift( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseShiftLeft, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseRightShift( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseShiftRight, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opLessThan( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareLess, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opLessThenOrEqual( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareLessEqual, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opGreaterThan( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareGreater, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opGreaterThanOrEqual( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareGraterEqual, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opEqualTo( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareEqual, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opNotEqualTo( Operand left, Operand right ) -> Operand {

    code::Type type( 1, false );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareNotEqual, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseAnd( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseAnd, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseXor( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseXor, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseOr( Operand left, Operand right ) -> Operand {

    code::Type type( left.type() );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseOr, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opAssignment( Operand left, Operand right ) -> Operand {

    code::Type toStore( left.type() );
    toStore.removeIndirection();
    if ( !right.isRegister() && right.value() == 0 && !toStore.isElementary() ) {
        right = Operand( code::Register( "null", toStore ) );
    }

    addInstruction( code::InstructionName::Store, {
        right,
        left
    } );

    return left;
}

auto Intermediate::opCompoundAssignment( Operand left, Operand right, common::Operator op ) -> Operand {
    code::Type type( left.type() );
    type.removeIndirection();
    Operand loaded( newRegister( type ) );

    addInstruction( code::InstructionName::Load, {
        loaded,
        left
    } );

    Operand result( 0, code::Type( 0, false ) );

    switch ( op ) {
    case common::Operator::Multiplication:
        result = opMultiplication( loaded, right );
        break;
    case common::Operator::Division:
        result = opDivision( loaded, right );
        break;
    case common::Operator::Modulo:
        result = opModulo( loaded, right );
        break;
    case common::Operator::Addition:
        result = opAddition( loaded, right );
        break;
    case common::Operator::Subtraction:
        result = opSubtraction( loaded, right );
        break;
    case common::Operator::BitwiseLeftShift:
        result = opBitwiseLeftShift( loaded, right );
        break;
    case common::Operator::BitwiseRightShift:
        result = opBitwiseRightShift( loaded, right );
        break;
    case common::Operator::BitwiseAnd:
        result = opBitwiseAnd( loaded, right );
        break;
    case common::Operator::BitwiseXor:
        result = opBitwiseXor( loaded, right );
        break;
    case common::Operator::BitwiseOr:
        result = opBitwiseOr( loaded, right );
        break;
    }

    return opAssignment( left, result );
}

} // namespace generator
} // namespace compiler
