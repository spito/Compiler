#include "Main.h"
#include "../preprocessor/Preprocessor.h"
#include "../preprocessor/Output.h"

int main( int argc, char **argv ) {
    return compiler::tool::Main( argc, argv ).run();
}

namespace compiler {
namespace tool {

int Main::run() {
    try {
        std::vector< std::string > cmds;
        for ( int i = 1; i < _argc; ++i )
            cmds.push_back( _argv[ i ] );

        ArgumentParser a( std::move( cmds ) );
        _meta = a.meta();

        switch ( _meta.mode ) {
        case Meta::Mode::Compiler:
            return compiler();
        case Meta::Mode::EmitASM:
            return assembler();
        case Meta::Mode::EmitLLVM:
            return llvm();
        case Meta::Mode::Interpret:
            return interpret();
        case Meta::Mode::Preprocessor:
            return preprocessor();
        }

    }
    catch ( exception::Exception &e ) {
        std::cerr << "Exception: " << e.what() <<
            " at " << e.position().line() << ":" <<
            e.position().column() << std::endl;
        return 1;
    }
    catch ( std::exception &e ) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 2;
    }
    catch ( ... ) {
        std::cerr << "Unknown exception" << std::endl;
        return 3;
    }
    return 0;
}

int Main::compiler() {
    throw exception::InternalError( "not implemented" );
}
int Main::preprocessor() {
    if ( _meta.input.size() != 1 )
        throw exception::InternalError( "invalid size of options (preprocessor)" );

    compiler::preprocessor::Preprocessor p( _meta.input.front() );
    compiler::preprocessor::Output o( p.store() );
    o.save( _meta.output );
    return 0;
}
int Main::interpret(){
    throw exception::InternalError( "not implemented" );
}
int Main::llvm(){
    throw exception::InternalError( "not implemented" );
}
int Main::assembler(){
    throw exception::InternalError( "not implemented" );
}
} // namesapce tool
} // namespace compiler
