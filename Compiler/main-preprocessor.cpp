#include "preprocessor/Output.h"

int main() {
    try {
        compiler::preprocessor::Preprocessor p( "Text.txt" );
        compiler::preprocessor::Output o( p );
        o.save( "out.txt" );
    }
    catch ( compiler::exception::Exception &e ) {
        std::cout << "Exception: " << e.what() <<
            " at " << e.position().line() << ":" <<
            e.position().column() << std::endl;
    }
    return 0;
}
