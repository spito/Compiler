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

}

} // namespace preprocessor
} // namespace compiler


