#pragma once

#include "../ast/Traversal.h"
#include "../common/AutoCheckPointer.h"
#include "Information.h"
#include "Frame.h"

#include <unordered_map>

namespace compiler {
namespace interpret {

struct Interpret : ast::Traversal< Frame > {


    Interpret( const Tree &tree ) :
        Base( tree )
    {}

    void start();

private:
    Variable findSymbol( const std::string & );

    Information intrinsicPrintf( std::vector< common::Register > );
    Information intrinsicScanf( std::vector< common::Register > );
    Information intrinsicPutc( std::vector< common::Register > );
    Information intrinsicMalloc( std::vector< common::Register > );
    Information intrinsicRealloc( std::vector< common::Register > );
    Information intrinsicFree( std::vector< common::Register > );

    bool checkRange( const void * );

    Information eval( const ast::Statement * );

    Information eval( const ast::Constant * );
    Information eval( const ast::StringPlaceholder * );
    Information eval( const ast::ArrayInitializer * );
    Information eval( const ast::Variable * );
    Information eval( const ast::UnaryOperator * );
    Information eval( const ast::BinaryOperator * );
    Information eval( const ast::TernaryOperator * );
    Information eval( const ast::Call * );

    Information eval( const ast::Block * );
    Information eval( const ast::If * );
    Information eval( const ast::Break * );
    Information eval( const ast::Continue * );
    Information eval( const ast::Return * );
    Information eval( const ast::While * );
    Information eval( const ast::DoWhile * );
    Information eval( const ast::For * );

    std::unordered_map< const char *, size_t > _whitelistPointers;
    bool _processingGlobal;
    static std::map< std::string, Information( Interpret::* )( std::vector< common::Register > ) > _intrinsicFunctions;

};

} // namespace interpret
} // namespace compiler
