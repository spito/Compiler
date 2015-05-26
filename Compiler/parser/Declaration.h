#pragma once

#include "Parser.h"
#include "SmartIterator.h"

namespace compiler {
namespace parser {

struct Declaration {

    enum class Type {
        None,
        TypeOnly,
        SingleVariable,
        Function,
        VariadicPack,
        Void
    };

    Declaration( Parser &p, SmartIterator &it ) :
        _it( it ),
        _begin( it ),
        _parser( p )
    {}

    const ast::type::Type *type() const {
        return _type;
    }
    const ast::Function *function() {
        return _function;
    }
    ast::Variable *variable() {
        return _variable.release();
    }

    Type decide( bool = true );
    SmartIterator begin() const {
        return _begin;
    }
    SmartIterator end() const {
        return _it;
    }

private:

    enum class States {
        Start,
        Typeword,
        Star,
        Array,
        Word,
        OpenParameters,
        CloseParameters,
        NestedDeclaration,
        LastNestedDeclaration,
        Comma,
        Quit,
        Error,
    };

    States stStart();
    States stStar();
    States stTypeword();
    States stWord();
    States stArray();
    States stOpenParametres();
    States stCloseParametres();
    States stDeclaration();
    States stDeclarationLast();
    States stComma();
    States stOpenBody();
    States stSemicolon();
    void stError();

    States toTypeword();
    States toStar();
    States toArray();
    States toWord();
    States toOpenParametres();
    States toCloseParametres();
    States toDeclaration();
    States toComma();
    States toError();

    States beTypeOnly();
    States beNone();
    States beVariable();
    States beVariadicPack();
    States beFunction( bool );
    States beVoid();

    States quit();

    SmartIterator &_it;
    SmartIterator _begin;
    Parser &_parser;

    ast::Function *_function;
    std::unique_ptr< ast::Variable > _variable;
    const ast::type::Type *_type = nullptr;

    std::vector< std::string > _typeId;
    std::string _name;
    std::vector< const ast::type::Type * > _types;
    std::vector< std::pair< std::string, const ast::type::Type * > > _parametres;
    Type _declarationType = Type::None;
    bool _quit = false;
    bool _void = false;
    bool _constness = false;
    bool _wait = false;
    bool _fullExpression;
};

} // namespace parser
} // namespace compiler
