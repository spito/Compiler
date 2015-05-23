#pragma once

#include "../ast/AST.h"
#include "../common/AutoCheckPointer.h"
#include "Information.h"
#include "Frame.h"

namespace compiler {
namespace interpret {

struct Interpret : ast::Traversal {

    std::list< Frame > _frames;
    std::unordered_map< const char *, size_t > _whitelistPointers;
    ast::AST &_ast;
    bool _processingGlobal;
    common::AutoCheckPointer< Information > _info;
    static std::map< std::string, void( Interpret::* )( std::vector< common::Register > ) > _intrinsicFunctions;

    Interpret( ast::AST &ast ) :
        _ast( ast )
    {}

    using FrameIterator = std::list< Frame >::reverse_iterator;

    void start();
    Variable findSymbol( const std::string & );
    void addRegister();
    void addRegister( bool( FrameIterator ) );

    void intrinsicPrintf( std::vector< common::Register > );
    void intrinsicScanf( std::vector< common::Register > );
    void intrinsicPutc( std::vector< common::Register > );

    bool checkRange( const void * );

    void eval( const ast::Statement * ) override;

    void eval( const ast::Constant * );
    void eval( const ast::StringPlaceholder * );
    void eval( const ast::Variable * );
    void eval( const ast::UnaryOperator * );
    void eval( const ast::BinaryOperator * );
    void eval( const ast::TernaryOperator * );
    void eval( const ast::Call * );

    void eval( const ast::Block * );
    void eval( const ast::If * );
    void eval( const ast::Break * );
    void eval( const ast::Continue * );
    void eval( const ast::Return * );
    void eval( const ast::While * );
    void eval( const ast::DoWhile * );
    void eval( const ast::For * );

};

} // namespace interpret
} // namespace compiler
