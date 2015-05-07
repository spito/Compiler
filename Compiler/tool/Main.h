#include "ArgumentParser.h"
#include "../includes/exceptions.h"

#include <iostream>

namespace compiler {
namespace tool {

struct Main {

    Main( int argc, char **argv ) :
        _argc( argc ),
        _argv( argv )
    {}

    int run();
private:
    int compiler();
    int preprocessor();
    int interpret();
    int llvm();
    int assembler();

    int _argc;
    char **_argv;
    Meta _meta;
};

} // namesapce tool
} // namespace compiler
