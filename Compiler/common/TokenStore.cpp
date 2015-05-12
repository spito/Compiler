#include "TokenStore.h"

namespace compiler {
namespace common {

std::unordered_map< std::string, Keyword > TokenStore::_keywords = {
        { "auto", Keyword::Auto },
        { "break", Keyword::Break },
        { "case", Keyword::Case },
        { "char", Keyword::Char },
        { "const", Keyword::Const },
        { "continue", Keyword::Continue },
        { "default", Keyword::Default },
        { "do", Keyword::Do },
        { "double", Keyword::Double },
        { "else", Keyword::Else },
        { "enum", Keyword::Enum },
        { "extern", Keyword::Extern },
        { "float", Keyword::Float },
        { "for", Keyword::For },
        { "goto", Keyword::Goto },
        { "if", Keyword::If },
        { "inline", Keyword::Inline },
        { "int", Keyword::Int },
        { "long", Keyword::Long },
        { "register", Keyword::Register },
        { "restrict", Keyword::Restrict },
        { "return", Keyword::Return },
        { "short", Keyword::Short },
        { "signed", Keyword::Signed },
        { "sizeof", Keyword::Sizeof },
        { "static", Keyword::Static },
        { "struct", Keyword::Struct },
        { "switch", Keyword::Switch },
        { "typedef", Keyword::Typedef },
        { "union", Keyword::Union },
        { "unsigned", Keyword::Unsigned },
        { "void", Keyword::Void },
        { "volatile", Keyword::Volatile },
        { "while", Keyword::While },
};

} // namespace common
} // namespace compiler
