#include "Parser.h"
#include "Statement.h"
#include "TypeChecker.h"

namespace compiler {
namespace parser {

void Parser::obtain() {

    SmartIterator it( _store.begin() );

    Statement stmt( *this, it );
    
    auto v = stmt.block();
    tree().global().import( *v );
    delete v;

    TypeChecker( tree() ).start();
}

std::unordered_set< std::string > Parser::_typeKeywords = {
    "void",
    "char",
    "short",
    "int",
    "long",
    "signed",
    "unsigned",
    "const"
};

std::unordered_set< std::string > Parser::_statementKeywords = {
    "if",
    "do",
    "for",
    "while",
    "break",
    "continue",
    "return"
};

} // namespace parser
} // namespace compiler
