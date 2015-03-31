#include "preprocessor/Preprocessor.h"

int main() {
    try {
        compiler::preprocessor::Preprocessor p( "Text.txt" );
        std::ofstream f( "out.txt" );
        f << p.content();
    }
    catch ( compiler::exception::Exception &e ) {
        std::cout << "Exception: " << e.what() <<
            " at " << e.position().line() << ":" <<
            e.position().column() << std::endl;
    }
    return 0;
}
