#pragma once

#include "../common/SymbolTable.h"
#include "../common/TokenStore.h"
#include "Symbol.h"

namespace compiler {
namespace preprocessor {

struct Global {
    common::SymbolTable< Symbol > symbols;
    common::TokenStore store;
    std::unordered_set< const std::string * > seenFiles;
};


} // namespace preprocessor
} // namespace compiler
