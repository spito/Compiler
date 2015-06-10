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


    for ( const code::Prototype &p : code.declarations() ) {
        writeDeclaration( p );
    }
}

std::string LLVM::getType( const code::Type &type, bool dots ) {

    if ( type.bits() == 0 )
        return dots ? "..." : "void";

    std::string result;

    for ( int d : common::revert( type.decoration() ) ) {
        if ( d != code::Type::Pointer ) {
            result += "[";
            result += std::to_string( d );
            result += " x ";
        }
    }

    result += 'i';
    result += std::to_string( type.bits() );

    for ( int d : type.decoration() ) {
        if ( d == code::Type::Pointer )
            result += '*';
        else
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
    return _function->basicBlocks().find( int( operand.value() ) )->second.name();
}

void LLVM::writeFunction( const code::Function &function ) {
    _function = &function;
    writeFormatted( "define # #(", getType( function.returnType() ), function.name() );

    bool first = true;
    for ( const code::Register &r : function.arguments() ) {
        if ( !first )
            write() << ", ";
        else
            first = false;

        write() << getOperand( r );
    }

    write() << ") {" << std::endl;

    for ( const code::Register &r : function.namedRegisters() ) {
        code::Type type( r.type() );
        type.removeIndirection();
        writeFormattedLine( "  # = alloca #", r.name(), getType( type ) );
    }

    std::map< int, int > table;
    for ( const auto &b : function.basicBlocks() ) {
        table[ b.second.order() ] = b.first;
    }
    for ( auto i : table ) {
        writeBlock( function.basicBlocks().find( i.second )->second );
    }

    writeFormattedLine( "}" );
}

void LLVM::writeBlock( const code::BasicBlock &block ) {

    writeFormatted( "; <label>:# ; preds = ", block.name() );

    bool first = true;
    for ( int p : block.predecessors() ) {
        if ( !first )
            write() << ", ";
        else
            first = false;


        write() << _function->basicBlocks().find( p )->second.name();
            
    }
    write() << std::endl;

    for ( const code::Instruction &i : block.instructions() ) {
        writeInstruction( i );
    }
}

void LLVM::writeInstruction( const code::Instruction &instruction ) {
    switch ( instruction.name() ) {
    case code::InstructionName::Global:
        writeFormatted( "# = private unnamed_addr constant # [",
                        getValue( instruction.operand( 0 ) ),
                        getType( instruction.operand( 0 ).type() ) );
        for ( int i = 1; i < int( instruction.size() ); ++i ) {
            if ( i > 1 )
                write() << ", ";
            write() << getOperand( instruction.operand( i ) );
        }
        write() << "]" << std::endl;
        break;
    case code::InstructionName::Alloc:
        writeFormattedLine( "  # = alloca #",
                            getValue( instruction.operand( 0 ) ),
                            getType( instruction.operand( 0 ).type() ) );
        break;
    case code::InstructionName::Load:
        writeFormattedLine( "  # = load #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ) );
        break;
    case code::InstructionName::Store:
        writeFormattedLine( "  store #, #",
                            getOperand( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ) );
        break;
    case code::InstructionName::AddressOf:
    case code::InstructionName::Dereference:
    case code::InstructionName::IndexAt:
        writeFormatted( "  # = getelementptr inbounds #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ) );
        for ( int i = 2; i < int( instruction.size() ); ++i )
            writeFormatted( ", #",
                            getOperand( instruction.operand( i ) ) );
        write() << std::endl;
        break;
    case code::InstructionName::Multiplication:
        writeFormattedLine( "  # = mul nsw #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Division:
        writeFormattedLine( "  # = sdiv #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Modulo:
        writeFormattedLine( "  # = srem #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Addition:
        writeFormattedLine( "  # = add nsw #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Subtraction:
        writeFormattedLine( "  # = sub nsw #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::BitwiseShiftLeft:
        writeFormattedLine( "  # = shl #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::BitwiseShiftRight:
        writeFormattedLine( "  # = ashr #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::BitwiseAnd:
        writeFormattedLine( "  # = and #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::BitwiseXor:
        writeFormattedLine( "  # = xor #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::BitwiseOr:
        writeFormattedLine( "  # = xor #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Extense:
        break;
    case code::InstructionName::Reduce:
        break;
    case code::InstructionName::PtrToInt:
        break;
    case code::InstructionName::CompareEqual:
        writeFormattedLine( "  # = icmp eq #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::CompareNotEqual:
        writeFormattedLine( "  # = icmp ne #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::CompareLess:
        writeFormattedLine( "  # = icmp slt #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::CompareLessEqual:
        writeFormattedLine( "  # = icmp sle #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::CompareGreater:
        writeFormattedLine( "  # = icmp sgt #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::CompareGraterEqual:
        writeFormattedLine( "  # = icmp sge #, #",
                            getValue( instruction.operand( 0 ) ),
                            getOperand( instruction.operand( 1 ) ),
                            getValue( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Jump:
        writeFormattedLine( "  br label #", 
                            getLabel( instruction.operand( 0 ) ) );
        break;
    case code::InstructionName::Branch:
        writeFormattedLine( "  br #, label #, label #",
                            getOperand( instruction.operand( 0 ) ),
                            getLabel( instruction.operand( 1 ) ),
                            getLabel( instruction.operand( 2 ) ) );
        break;
    case code::InstructionName::Merge:
        writeFormatted( "  # = phi # ",
                        getValue( instruction.operand( 0 ) ),
                        getType( instruction.operand( 0 ).type() ) );
        for ( int i = 1; i < int( instruction.size() ); i += 2 ) {
            if ( i > 1 )
                write() << ", ";

            writeFormatted( "[ #, # ]",
                            getValue( instruction.operand( i ) ),
                            getLabel( instruction.operand( i + 1 ) ) );
        }
        write() << std::endl;
        break;
    case code::InstructionName::Call:{
        int start = 0;
        write() << "  ";
        if ( instruction.operand( 0 ).type().bits() ) {
            writeFormatted( "# = ", getValue( instruction.operand( 0 ) ) );
            start = 1;
        }
        writeFormatted( "call # ", getType( instruction.operand( start ).type() ) );
        if ( auto d = _code->declaration( getValue( instruction.operand( start ) ) ) ) {
            if ( d->isVariadic() ) {
                write() << "(";
                bool first = true;
                for ( const code::Type &t : d->argumentTypes() ) {
                    if ( !first )
                        write() << ", ";
                    else
                        first = false;
                    write() << getType( t, true );
                }
                write() << ")* ";
            }
        }
        writeFormatted( "#(", getValue( instruction.operand( start ) ) );

        for ( int i = start + 1; i < int( instruction.size() ); ++i ) {
            if ( i > start + 1 )
                write() << ", ";

            write() << getOperand( instruction.operand( i ) );
        }
        write() << ")" << std::endl;
    }
        break;
    case code::InstructionName::Return:
        writeFormattedLine( "  ret #", getOperand( instruction.operand( 0 ) ) );
        break;

    }
}

void LLVM::writeDeclaration( const code::Prototype &prototype ) {

    writeFormatted( "declare # #(", 
                    getType( prototype.returnType() ),
                    prototype.name() );

    bool first = true;
    for ( const code::Type &t : prototype.argumentTypes() ) {
        if ( !first )
            write() << ", ";
        else
            first = false;
        write() << getType( t, true );
    }
    write() << ")" << std::endl;
}

} // namespace generator
} // namespace compiler
