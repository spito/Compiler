#pragma once

#include "../common/TokenStore.h"

namespace compiler {
namespace preprocessor {

struct Output {

    Output( const common::TokenStore &store ) :
        _store( store )
    {}

    void save( const char * ) const;
    void save( const std::string &name ) const {
        save( name.c_str() );
    }


private:
    const common::TokenStore &_store;

};

} // namespace preprocessor
} // namespace compiler

