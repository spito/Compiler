#pragma once

#include "Meta.h"

namespace compiler {
namespace tool {

struct ArgumentParser {

    ArgumentParser( std::vector< std::string > cmds ) {
        process( std::move( cmds ) );
    }

    Meta meta() const {
        return _meta;
    }
private:

    void process( std::vector< std::string > );

    Meta _meta;
};

} // namesapce tool
} // namespace compiler
