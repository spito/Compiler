#pragma once

#include "../common/Token.h"
#include "../common/SmartIterator.h"
#include "../includes/exceptions.h"


namespace compiler {
namespace preprocessor {

bool expression( common::SmartIterator< std::vector< common::Token >::iterator > && );

} // namespace preprocessor
} // namespace compiler

