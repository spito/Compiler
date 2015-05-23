#pragma once

#include "Parser.h"
#include "SmartIterator.h"

namespace compiler {
namespace parser {

struct Statement {

    using Operator = common::Operator;
    using Keyword = common::Keyword;

    Statement( Parser &p, SmartIterator &it ) :
        _it( it ),
        _parser( p )
    {}

    ast::Block *block();

private:

    ast::Statement *single();
    ast::If *ifStatement();
    ast::For *forStatement();
    ast::While *whileStatement();
    ast::DoWhile *doWhileStatement();
    ast::Break *breakStatement();
    ast::Continue *continueStatement();
    ast::Return *returnStatement();
    ast::Block *blockStatement();

    SmartIterator &_it;
    Parser &_parser;

};

} // namespace parser
} // namespace compiler
