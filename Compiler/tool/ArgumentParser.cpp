#include "ArgumentParser.h"
#include "../common/SmartIterator.h"
#include "../includes/exceptions.h"

#include <map>

namespace compiler {
namespace tool {

using Iterator = common::SmartIterator< std::vector< std::string >::iterator >;

void setPreprocessor( Meta &m, Iterator & ) {
    if ( m.mode != Meta::Mode::Compiler )
        throw exception::InternalError( "wrong option" );
    m.mode = Meta::Mode::Preprocessor;
}

void setInterpret( Meta &m, Iterator & ) {
    if ( m.mode != Meta::Mode::Compiler )
        throw exception::InternalError( "wrong option" );
    m.mode = Meta::Mode::Interpret;
}

void setLLVM( Meta &m, Iterator & ) {
    if ( m.mode != Meta::Mode::Compiler )
        throw exception::InternalError( "wrong option" );
    m.mode = Meta::Mode::EmitLLVM;
}
void setASM( Meta &m, Iterator & ) {
    if ( m.mode != Meta::Mode::Compiler )
        throw exception::InternalError( "wrong option" );
    m.mode = Meta::Mode::EmitASM;
}

void setOutput( Meta &m, Iterator &i ) {
    ++i;
    if ( i )
        m.output = *i;
}

static std::map< std::string, void(*)( Meta &, Iterator & ) > options = {
        { "-E", setPreprocessor },
        { "-i", setInterpret },
        { "-llvm", setLLVM },
        { "-asm", setASM },
        { "-o", setOutput },
};

void ArgumentParser::process( std::vector< std::string > cmds ) {

    Iterator i( cmds.begin(), cmds.end() );

    for ( ; i; ++i ) {
        auto o = options.find( *i );

        if ( o != options.end() )
            o->second( _meta, i );
        else
            _meta.input.push_back( *i );
    }
}

} // namesapce tool
} // namespace compiler
