#pragma once

#include "Writer.h"

namespace compiler {
namespace generator {

struct LLVM : Writer {

    LLVM( const char *filename ) :
        Writer( filename ),
        _code( nullptr )
    {}

    void publish( const code::Code & );

    void writeFunction( const code::Function & ) override;
    void writeBlock( const code::BasicBlock & ) override;
    void writeInstruction( const code::Instruction & ) override;
    void writeDeclaration( const code::Prototype & );
    std::string getType( const code::Type & );
    std::string getOperand( const code::Operand & );
    std::string getValue( const code::Operand & );
    std::string getLabel( const code::Operand & );
private:
    const code::Code *_code;
    const code::Function *_function;

};


} // namespace generator
} // namespace compiler
