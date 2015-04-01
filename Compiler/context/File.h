#pragma once

#include <string>

namespace compiler {
namespace context {

const std::string &emptyString();

const std::string &file();
void file( std::string );

} // namespace context
} // namespace compiler
