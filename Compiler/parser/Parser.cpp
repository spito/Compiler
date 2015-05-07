#include "Parser.h"
#include "Expression.h"
namespace compiler {
namespace parser {

void Parser::obtain() {
    SmartIterator it( _store.begin() );
    _ast.global().add( Expression( *this, it ) );
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
