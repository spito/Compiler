#include "Parser.h"
#include "Expression.h"
namespace compiler {
namespace parser {

void Parser::obtain() {
    _ast.global().add( Expression( *this, _store.begin() ) );
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
