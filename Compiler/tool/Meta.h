#pragma once

#include <string>
#include <vector>

namespace compiler {
namespace tool {

struct Meta {

    enum class Mode {
        Compiler,
        Preprocessor,
        EmitLLVM,
        EmitASM,
        Interpret
    };

    Meta() :
        mode( Mode::Compiler )
    {}

    Mode mode;
    std::string output;
    std::vector< std::string > input;
};

} // namesapce tool
} // namespace compiler
