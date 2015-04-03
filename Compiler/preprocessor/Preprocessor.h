#pragma once

#include "Global.h"

#include <string>
#include <memory>

namespace compiler {
namespace preprocessor {

struct Preprocessor {


    Preprocessor( const std::string & );

    size_t size() const {
        return store().size();
    }

    const common::TokenStore &store() const {
        return _global.store;
    }
    common::TokenStore &store() {
        return _global.store;
    }


    Preprocessor( const Preprocessor & ) = delete;


private:
    Global _global;

    void setPredefined();

};

} // namespace preprocessor
} // namespace compiler
