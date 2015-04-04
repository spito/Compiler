#include "Preprocessor.h"
#include "Worker.h"


namespace compiler {
namespace preprocessor {

Preprocessor::Preprocessor( const std::string &name ) {

    context::file( name );

    setPredefined();
    Worker w( name, _global );
}


void Preprocessor::setPredefined() {
    _global.symbols.assign( Symbol::makeDefined() );
    _global.symbols.assign( Symbol::makeSpecial( "__FILE__" ) );
    _global.symbols.assign( Symbol::makeSpecial( "__LINE__" ) );
}

} // namespace preprocessor
} // namespace compiler


