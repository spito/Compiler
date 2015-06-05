#pragma once

#include "../ast/Traversal.h"
#include "../code/Code.h"
#include "../common/LayeredMap.h"
#include "Frame.h"

#include <unordered_map>
#include <vector>

namespace compiler {
namespace generator {

struct Intermediate : ast::Traversal< Frame > {

    Intermediate( const Tree &tree ) :
        Base( tree )
    {}

    void start();

    const code::Code &code() const {
        return _code;
    }

private:

    using Operand = code::Operand;

    void resetRegisters() {
        _registerIndex = 0;
    }

    int registerIndex() {
        return _registerIndex++;
    }
    int globalIndex() {
        return _globalIndex++;
    }
    int nameIndex() {
        return _nameIndex++;
    }

    code::Register newRegister( code::Type );
    std::string labelName();
    std::string globalName();
    std::string uniqueName( std::string );


    common::Defer addNamedRegisters( const ast::MemoryHolder * );
    //Register addString(  )
    void addInstruction( code::InstructionName, std::vector< Operand > );
    void addJump( int );
    void addBranch( Operand, int, int );
    int addBasicBlock();

    code::BasicBlock &block( int );
    void refreshBlock( int );

    static code::Type convertType( const ast::TypeOf & );

    enum class Access : bool {
        Load,
        Store
    };

    void eval( const ast::Function * );
    bool eval( const ast::Statement * );
    bool eval( const ast::Block * );
    bool eval( const ast::If * );
    bool eval( const ast::While * );
    bool eval( const ast::DoWhile * );
    bool eval( const ast::For * );
    bool eval( const ast::Break * );
    bool eval( const ast::Continue * );
    bool eval( const ast::Return * );
    Operand eval( const ast::Expression *, Access = Access::Load );
    Operand eval( const ast::Constant * );
    Operand eval( const ast::StringPlaceholder * );
    Operand eval( const ast::ArrayInitializer * );
    Operand eval( const ast::Variable *, Access = Access::Load );
    Operand eval( const ast::UnaryOperator *, Access = Access::Load );
    Operand eval( const ast::BinaryOperator *, Access = Access::Load );
    Operand eval( const ast::TernaryOperator *, Access = Access::Load );
    Operand eval( const ast::Call * );

    Operand opLogicalNot( Operand );
    Operand opAddressOf( Operand );
    Operand opDereference( Operand );
    Operand opPrefixDecrement( Operand );
    Operand opPrefixIncrement( Operand );
    Operand opSuffixDecrement( Operand );
    Operand opSuffixIncrement( Operand );
    Operand opBitwiseNot( Operand );
    Operand opUnaryMinus( Operand );

    Operand opLogicalOr( const ast::BinaryOperator *, Access = Access::Load );
    Operand opLogicalAnd( const ast::BinaryOperator *, Access = Access::Load );

    Operand opArrayAccess( Operand, Operand, Access = Access::Load );
    Operand opTypeCast( code::Type, Operand );
    Operand opMultiplication( Operand, Operand );
    Operand opDivision( Operand, Operand );
    Operand opModulo( Operand, Operand );
    Operand opAddition( Operand, Operand );
    Operand opSubtraction( Operand, Operand );
    Operand opBitwiseLeftShift( Operand, Operand );
    Operand opBitwiseRightShift( Operand, Operand );
    Operand opLessThan( Operand, Operand );
    Operand opLessThenOrEqual( Operand, Operand );
    Operand opGreaterThan( Operand, Operand );
    Operand opGreaterThanOrEqual( Operand, Operand );
    Operand opEqualTo( Operand, Operand );
    Operand opNotEqualTo( Operand, Operand );
    Operand opBitwiseAnd( Operand, Operand );
    Operand opBitwiseXor( Operand, Operand );
    Operand opBitwiseOr( Operand, Operand );

    Operand opAssignment( Operand, Operand );
    Operand opCompoundAssignment( Operand, Operand, common::Operator );

    int _registerIndex;
    int _nameIndex;
    int _globalIndex;
    int _currentBlock;
    code::Code _code;
    std::vector< code::BasicBlock > _basicBlocks;
    std::vector< code::Register > _namedRegisters;
    std::vector< code::Instruction > _globals;
    common::LayeredMap< std::string, unsigned > _nameMapping;
};

} // namespace generator
} // namespace compiler
