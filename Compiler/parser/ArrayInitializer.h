#pragma once

#include "Parser.h"
#include "SmartIterator.h"
#include "../ast/ArrayInitiaizer.h"

#include <stack>
#include <vector>

namespace compiler {
namespace parser {

struct ArrayInitializer {

    ArrayInitializer( Parser &p, SmartIterator &it ) : 
        _begin( it ),
        _it( it ),
        _parser( p )
    {}

    ast::ArrayInitializer *obtain( ast::Variable *variable, ast::TypeOf type ) {
        _type = std::move( type );
        _variable.reset( variable );
        dimensions();
        _quit = false;

        return descend();
    }

private:

    using Operator = common::Operator;

    enum class States {
        Start,
        BraceOpen,
        Value,
        CommaInside,
        CommaOutside,
        BraceClose,
        Quit,
        Error,
    };

    ast::ArrayInitializer *descend();

    States stStart();
    States stBraceOpen();
    States stValue();
    States stCommaInside();
    States stCommaOutside();
    States stBraceClose();
    void stError();

    States toFirstBraceOpen();
    States toNextBraceOpen();
    States toValue();
    States toStringValue();
    States toCommaInside();
    States toCommaOutside();
    States toBraceClose();
    States toError();
    States toQuit();

    States beString();


    void dimensions();

    SmartIterator _begin;
    SmartIterator &_it;
    Parser &_parser;

    ast::TypeOf _type;
    const ast::TypeOf *_baseType;
    std::unique_ptr< ast::Variable > _variable;

    std::vector< ast::Expression::EHandle > _values;
    std::vector< int > _dimensions;
    std::vector< int > _currentIndex;
    bool _quit;
    bool _wait = false;
};

} // namespace parser
} // namespace compiler
