#include "Intermediate.h"

namespace compiler {
namespace generator {

code::Type Intermediate::unification( Operand &lhs, Operand &rhs ) {
    if ( lhs.type().bits() == rhs.type().bits() )
        return lhs.type();

    if ( lhs.type().bits() < rhs.type().bits() ) {
        code::Operand result( newRegister( rhs.type() ) );
        addInstruction( code::InstructionName::ExtenseUnsigned, {
            result,
            lhs
        } );
        lhs = result;
        return rhs.type();
    }
    else {
        code::Operand result( newRegister( lhs.type() ) );
        addInstruction( code::InstructionName::ExtenseUnsigned, {
            result,
            rhs
        } );
        rhs = result;
        return lhs.type();
    }
}

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

    Operand pointer;

    type.removeIndirection();
    if ( type.isArray() ) {
        type.removeDimension();
        type.addIndirection();

        pointer = Operand( newRegister( type ) );
        addInstruction( code::InstructionName::IndexAt, {
            pointer,
            left,
            Operand( 0, code::Type( 32 ) ),
            right
        } );
    }
    else {
        type.addIndirection();

        pointer = Operand( newRegister( type ) );
        addInstruction( code::InstructionName::IndexAt, {
            pointer,
            left,
            right
        } );
    }


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

auto Intermediate::opTypeCast( code::Type targetType, Operand input, Casting casting ) -> Operand {

    auto rank = []( code::Type t ) -> int {
        if ( t.isElementary() )
            return 0;
        if ( t.isArray() )
            return 1;
        t.removeIndirection();
        if ( t.isArray() )
            return 1;
        return 2;
    };

    /*
   Implicit:
   | \  to | E | A | P
   |   \   |   |   |
   |from \ |   |   |
   |-------------------
   |E      | a | x | nullOnly
   |-------------------
   |A      | x | x | bitcast (same base)
   |-------------------
   |P      | x | x | x

   Explicit:
   | \  to | E | A | P
   |   \   |   |   |
   |from \ |   |   |
   |-------------------
   |E      | a | x | nullOnly + sext
   |-------------------
   |A      | x | x | bitcast
   |-------------------
   |P      |p2i| x | bitcast

   rank( from ) * 3 + rank( to )

   */

    enum Table {
        EtoE = 0,
        EtoP = 0 * 3 + 2,
        AtoP = 1 * 3 + 2,
        PtoE = 2 * 3 + 0,
        PtoP = 2 * 3 + 2
    };

    switch ( rank( input.type() ) * 3 + rank( targetType ) ) {
    case EtoE: {
        if ( !input.isRegister() )
            return Operand( input.value(), targetType );

        Operand result( newRegister( targetType ) );
        code::InstructionName instruction =
            input.type().bits() < targetType.bits() ?
            code::InstructionName::ExtenseUnsigned :
            code::InstructionName::Reduce;

        addInstruction( instruction, {
            result,
            input
        } );
        return result;
    }
        break;
    case EtoP:
        if ( !input.isRegister() && input.value() == 0 )
            return Operand( code::Register( "null", targetType ) );
        if ( casting == Casting::Explicit ) {

            Operand result( newRegister( targetType ) );
            addInstruction( code::InstructionName::ExtenseSigned, {
                result,
                input
            } );
            return result;
        }
        break;
    case AtoP: {
        code::Type type( input.type() );
        if ( type.isPointer() )
            type.removeIndirection();
        type.removeDimension();

        if ( !type.isElementary() )
            break;
        type.addIndirection();

        Operand partial( newRegister( type ) );
        addInstruction( code::InstructionName::BitCast, {
            partial,
            input
        } );

        if ( type == targetType )
            return partial;
        else if ( casting == Casting::Explicit ) {

            Operand result( newRegister( targetType ) );
            addInstruction( code::InstructionName::BitCast, {
                result,
                partial
            } );
            return result;
        }
    }
        break;
    case PtoE:
        if ( casting == Casting::Explicit ) {

            Operand result( newRegister( targetType ) );
            addInstruction( code::InstructionName::PtrToInt, {
                result,
                input
            } );
            return result;
        }
        break;
    case PtoP:
        if ( casting == Casting::Explicit ) {
            Operand result( newRegister( targetType ) );
            addInstruction( code::InstructionName::BitCast, {
                result,
                input
            } );
            return result;
        }
        break;
    default:
        break;
    }
    throw exception::InternalError( "cannot perform such cast" );
}

auto Intermediate::opMultiplication( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Multiplication, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opDivision( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Division, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opModulo( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Modulo, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opAddition( Operand left, Operand right ) -> Operand {

    if ( left.type().isPointer() ) {
        Operand result( newRegister( left.type() ) );
        addInstruction( code::InstructionName::IndexAt, {
            result,
            left,
            right
        } );
        return result;
    }
    if ( right.type().isPointer() ) {
        Operand result( newRegister( right.type() ) );
        addInstruction( code::InstructionName::IndexAt, {
            result,
            right,
            left
        } );
        return result;
    }

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Addition, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opSubtraction( Operand left, Operand right ) -> Operand {

    if ( left.type().isPointer() && right.type().isPointer() ) {
        left = opTypeCast( code::Type( 64 ), left, Casting::Explicit );
        right = opTypeCast( code::Type( 64 ), right, Casting::Explicit );
    }

    code::Type type( unification( left, right ) );

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

    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );

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
    
    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );

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

    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );

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

    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );
    
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

    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );

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

    if ( !left.type().isElementary() )
        left = opTypeCast( code::Type( 64 ), left );
    if ( !right.type().isElementary() )
        right = opTypeCast( code::Type( 64 ), right );
    unification( left, right );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::CompareNotEqual, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseAnd( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseAnd, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseXor( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

    Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::BitwiseXor, {
        result,
        left,
        right
    } );
    return result;
}

auto Intermediate::opBitwiseOr( Operand left, Operand right ) -> Operand {

    code::Type type( unification( left, right ) );

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

    if ( left.type() != toStore )
        right = opTypeCast( toStore, right );

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
    default:
        throw exception::InternalError( "cannot happen" );
    }

    return opAssignment( left, result );
}

} // namespace generator
} // namespace compiler
