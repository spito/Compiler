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
        _scopes.push_back( &_ast.global() );
        _blocks.push( &_ast.global() );
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

    bool addVariable( std::string name, ast::TypeOf type ) {
        return _scopes.back()->add( std::move( name ), std::move( type ) );
    }

    bool isValidVariable( const std::string &name ) {
        for ( auto i = _scopes.rbegin(); i != _scopes.rend(); ++i ) {
            if ( ( *i )->hasVariable( name ) )
                return true;
        }
        return false;
    }

    const ast::MemoryHolder::Variable *getVariable( const std::string &name ) {
        for ( auto i = _scopes.rbegin(); i != _scopes.rend(); ++i ) {
            auto r = ( *i )->getVariable( name );
            if ( r )
                return r;
        }
        return nullptr;
    }

    bool insideFunction() const {
        return !_functions.empty();
    }

    bool insideCycle() const {
        return _nestedCycles > 0;
    }

    common::Defer openCycle( ast::MemoryHolder *s ) {
        ++_nestedCycles;
        if ( s )
            _scopes.push_back( s );
        return common::Defer( [s,this] {
            --_nestedCycles;
            if ( s )
                _scopes.pop_back();
        } );
    }

    common::Defer openFunction( ast::Function *f ) {
        _functions.push_back( f );
        _blocks.push( &f->body() );
        _scopes.push_back( &f->parameters() );
        _scopes.push_back( &f->body() );

        return common::Defer( [this] { 
            _scopes.pop_back();
            _scopes.pop_back();
            _blocks.pop();
            _functions.pop_back();
        } );
    }
    common::Defer openScope( ast::MemoryHolder *s ) {
        _scopes.push_back( s );
        return common::Defer( [this] { _scopes.pop_back(); } );
    }
    common::Defer openBlock( ast::Block *b ) {
        _blocks.push( b );
        _scopes.push_back( b );
        return common::Defer( [this] {
            _scopes.pop_back();
            _blocks.pop();
        } );
    }

    ast::Block *block() {
        return _blocks.top();
    }

    void addFunction( ast::Function *f ) {
        tree().add( ast::AST::FunctionHandle( f ) );
    }

    ast::Function *function() {
        if ( !insideFunction() )
            return nullptr;
        return _functions.back();
    }

private:

    void obtain();

    ast::AST _ast;
    common::TokenStore &_store;
    std::vector< ast::MemoryHolder * > _scopes;
    std::stack< ast::Block * > _blocks;
    std::vector< ast::Function * > _functions;
    int _nestedCycles = 0;

    static std::unordered_set< std::string > _typeKeywords;
    static std::unordered_set< std::string > _statementKeywords;
};

} // namespace parser
} // namespace compiler
