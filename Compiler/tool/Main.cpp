#include "Main.h"
#include "../preprocessor/Preprocessor.h"
#include "../preprocessor/Output.h"
#include "../parser/Parser.h"
#include "../interpret/Interpret.h"
#include "../generator/Intermediate.h"
#include "../generator/LLVM.h"

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
    if ( _meta.input.size() != 1 )
        throw exception::InternalError( "invalid size of options (preprocessor)" );

    compiler::preprocessor::Preprocessor p( _meta.input.front() );
    compiler::parser::Parser parser( p.store() );

    compiler::generator::Intermediate i( parser.tree() );
    i.start();

    std::string intermediate( _meta.input.front() + ".ll" );
    compiler::generator::LLVM g( intermediate.c_str() );
    g.publish( i.code() );

    std::string cmd( "clang " );

    cmd += intermediate;
    cmd += " -o ";
    cmd += _meta.output;


    std::cout << "running external command:" << std::endl << '\t' << cmd << std::endl;
    std::system( cmd.c_str() );
    return 0;
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
    
    if ( _meta.input.size() != 1 )
        throw exception::InternalError( "invalid size of options (preprocessor)" );

    compiler::preprocessor::Preprocessor p( _meta.input.front() );
    compiler::parser::Parser parser( p.store() );

    compiler::interpret::Interpret i( parser.tree() );

    i.start();
    return 0;
}
int Main::llvm(){
    if ( _meta.input.size() != 1 )
        throw exception::InternalError( "invalid size of options (preprocessor)" );

    compiler::preprocessor::Preprocessor p( _meta.input.front() );
    compiler::parser::Parser parser( p.store() );

    compiler::generator::Intermediate i( parser.tree() );
    i.start();

    compiler::generator::LLVM g( _meta.output.c_str() );
    g.publish( i.code() );
    return 0;

}
int Main::assembler(){
    throw exception::InternalError( "not implemented" );
}
} // namesapce tool
} // namespace compiler
