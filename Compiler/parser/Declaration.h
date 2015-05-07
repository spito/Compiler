#pragma once

#include "Parser.h"

namespace compiler {
namespace parser {

struct Declaration {

    enum class Type {
        None,
        TypeOnly,
        SingleVariable,
        Function,
        VariadicPack
    };

    using SmartIterator = common::TokenStore::Iterator;

    Declaration( Parser &p, SmartIterator &it ) :
        _it( it ),
        _begin( it ),
        _parser( p )
    {}

    const ast::type::Type *typeOnly() const {
        return _type;
    }
    ast::Function function() {
        if ( !_function )
            throw exception::InternalError( "invalid call" );
        ast::Function f( std::move( *_function ) );
        delete _function;
        _function = nullptr;
        return f;
    }
    ast::Variable *variable() {
        auto v = _variable;
        _variable = nullptr;
        return v;
    }

    Type decide();
    SmartIterator begin() {
        return _begin;
    }
    SmartIterator end() {
        return _it;
    }

private:
    SmartIterator &_it;
    SmartIterator _begin;
    Parser &_parser;

    ast::Function *_function = nullptr;
    ast::Variable *_variable = nullptr;
    const ast::type::Type *_type = nullptr;
};

} // namespace parser
} // namespace compiler
