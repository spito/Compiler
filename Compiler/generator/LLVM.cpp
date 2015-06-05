#include "LLVM.h"

namespace compiler {
namespace generator {

void LLVM::publish( const code::Code &code ) {

    _code = &code;

    write() << "target datalayout = \"e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128\"" << std::endl;
    write() << "target triple = \"x86_64-unknown-linux-gnu\"\n" << std::endl;


    for ( const code::Instruction &i : code.globals() ) {
        writeInstruction( i );
    }


    for ( const auto &f : code.functions() ) {
        writeFunction( f.second );
    }
}

std::string LLVM::getType( const code::Type &type ) {

    if ( type.bits() == 0 )
        return "void";

    std::string result;

    for ( int d : type.dimensions() ) {
        result += "[";
        result += std::to_string( d );
        result += " x ";
    }

    result += 'i';
    result += std::to_string( type.bits() );

    for ( int i = 0; i < type.indirection(); ++i ) {
        result += '*';
    }

    for ( int d : type.dimensions() ) {
        result += ']';
    }
    return result;
}

std::string LLVM::getOperand( const code::Operand &operand ) {
    if ( operand.type().bits() == 0 )
        return "void";
    return getType( operand.type() ) + " " + getValue( operand );
}
std::string LLVM::getValue( const code::Operand &operand ) {
    return operand.isRegister() ?
        operand.name() :
        std::to_string( operand.value() );
}


std::string LLVM::getLabel( const code::Operand &operand ) {
    return _function->basicBlocks()[ size_t( operand.value() ) ].name();
}

void LLVM::writeFunction( const code::Function &function ) {
    _function = &function;
    writeFormattedLine( "define # @#() {", getType( function.returnType() ), function.name() );

    for ( const code::Register &r : function.namedRegisters() ) {
        code::Type type( r.type() );
        type.removeIndirection();
        writeFormattedLine( "  # = alloca #", r.name(), getType( type ) );
    }

    for ( const code::BasicBlock &b : function ) {
        writeBlock( b );
    }

    writeFormattedLine( "}" );
}

void LLVM::writeBlock( const code::BasicBlock &block ) {

    writeFormatted( "; <label>:# ; preds = ", block.name() );

    bool first = true;
    for ( int p : block.predecessors() ) {
        if ( !first )
            write() << ", ";
        writeFormatted( "#", _function->basicBlocks()[ p ].name() );
        first = false;
    }
    write() << std::endl;

    for ( const code::Instruction &i : block.instructions() ) {
        writeInstruction( i );
    }
}

void LLVM::writeInstruction( const code::Instruction &instruction ) {
    switch ( instruction.name() ) {
    case code::InstructionName::Global:
        break;
    case code::InstructionName::Alloc:
        writeFormattedLine( "  # = alloca #",
                            getValue( instruction.operands().front() ),
                            getType( instruction.operands().front().type() ) );
        break;
    case code::InstructionName::Load:
        writeFormattedLine( "  # = load #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ) );
        break;
    case code::InstructionName::Store:
        writeFormattedLine( "  store #, #",
                            getOperand( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ) );
        break;
    case code::InstructionName::AddressOf:
    case code::InstructionName::Dereference:
    case code::InstructionName::IndexAt:
        writeFormattedLine( "  # = getelementptr inbounds #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getOperand( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Multiplication:
        writeFormattedLine( "  # = mul nsw #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Division:
        writeFormattedLine( "  # = sdiv #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Modulo:
        writeFormattedLine( "  # = srem #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Addition:
        writeFormattedLine( "  # = add nsw #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Subtraction:
        writeFormattedLine( "  # = sub nsw #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::BitwiseShiftLeft:
        writeFormattedLine( "  # = shl #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::BitwiseShiftRight:
        writeFormattedLine( "  # = ashr #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::BitwiseAnd:
        writeFormattedLine( "  # = and #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::BitwiseXor:
        writeFormattedLine( "  # = xor #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::BitwiseOr:
        writeFormattedLine( "  # = xor #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Extense:
        break;
    case code::InstructionName::Reduce:
        break;
    case code::InstructionName::PtrToInt:
        break;
    case code::InstructionName::CompareEqual:
        writeFormattedLine( "  # = icmp eq #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::CompareNotEqual:
        writeFormattedLine( "  # = icmp ne #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::CompareLess:
        writeFormattedLine( "  # = icmp slt #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::CompareLessEqual:
        writeFormattedLine( "  # = icmp sle #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::CompareGreater:
        writeFormattedLine( "  # = icmp sgt #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::CompareGraterEqual:
        writeFormattedLine( "  # = icmp sge #, #",
                            getValue( instruction.operands()[ 0 ] ),
                            getOperand( instruction.operands()[ 1 ] ),
                            getValue( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Jump:
        writeFormattedLine( "  br label #", 
                            getLabel( instruction.operands()[ 0 ] ) );
        break;
    case code::InstructionName::Branch:
        writeFormattedLine( "  br #, label #, label #",
                            getOperand( instruction.operands()[ 0 ] ),
                            getLabel( instruction.operands()[ 1 ] ),
                            getLabel( instruction.operands()[ 2 ] ) );
        break;
    case code::InstructionName::Merge:
        writeFormatted( "  # = phi # ", instruction.operands()[ 0 ].name() );
        for ( int i = 1; i < int( instruction.operands().size() ); i += 2 ) {
            if ( i > 1 )
                write() << ", ";

            writeFormatted( "[ # # ]",
                            getLabel( instruction.operands()[ i ] ),
                            instruction.operands()[ i + 1 ].name() );
        }
        break;
    case code::InstructionName::Call:
    case code::InstructionName::Return:
        writeFormattedLine( "  ret #", getOperand( instruction.operands()[ 0 ] ) );
        break;

    }
}

} // namespace generator
} // namespace compiler
