#pragma once

#include "../common/TokenStore.h"

namespace compiler {
namespace preprocessor {

struct Output {

    Output( common::TokenStore &store ) :
        _store( store )
    {}

    void save( const char * );
    void save( const std::string &name ) {
        save( name.c_str() );
    }


private:
    common::TokenStore &_store;

};

} // namespace preprocessor
} // namespace compiler

