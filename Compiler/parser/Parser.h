#pragma once

#include "../common/TokenStore.h"
#include "../ast/AST.h"

#include <unordered_set>
#include <stack>

namespace compiler {
namespace parser {

struct Parser {

    Parser( common::TokenStore &store ) :
        _store( store )
    {
        _blocks.push_back( &_ast.global() );
        obtain();
    }

    ast::AST &tree() {
        return _ast;
    }

    ast::TypeStorage &typeStorage() {
        return _ast.typeStorage();
    }

    bool isTypeKeyword( const std::string &word ) const {
        return _typeKeywords.find( word ) != _typeKeywords.end();
    }

    bool isStatementKeyword( const std::string &word ) const {
        return _statementKeywords.find( word ) != _statementKeywords.end();
    }

    bool addVariable( std::string name, const ast::type::Type *type ) {
        return _blocks.back()->add( std::move( name ), type );
    }

    bool isValidVariable( const std::string &name ) {
        for ( auto i = _blocks.rbegin(); i != _blocks.rend(); ++i ) {
            if ( ( *i )->hasVariable( name ) )
                return true;
        }
        return false;
    }

    const ast::MemoryHolder::Variable *getVariable( const std::string &name ) {
        for ( auto i = _blocks.rbegin(); i != _blocks.rend(); ++i ) {
            auto r = ( *i )->getVariable( name );
            if ( r )
                return r;
        }
        return nullptr;
    }

private:

    void obtain();

    ast::AST _ast;
    common::TokenStore &_store;
    std::vector< ast::Block * > _blocks;

    static std::unordered_set< std::string > _typeKeywords;
    static std::unordered_set< std::string > _statementKeywords;
};

} // namespace parser
} // namespace compiler
