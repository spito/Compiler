#pragma once

#include "Preprocessor.h"

namespace compiler {
namespace preprocessor {

struct Output {

    Output( Preprocessor &p ) :
        _p( p )
    {}

    void save( const char * );
    void save( const std::string &name ) {
        save( name.c_str() );
    }


private:
    Preprocessor &_p;

};

} // namespace preprocessor
} // namespace compiler

