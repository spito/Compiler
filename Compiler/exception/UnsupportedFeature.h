#pragma once

#include "Exception.h"

#include <string>

namespace compiler {
    namespace exception {

        class UnsupportedFeature : public Exception {
            common::Position _position;
            std::string _msg;
        public:
            UnsupportedFeature( const char *msg, common::Position p ) :
                Exception( "" ),
                _position( std::move( p ) ),
                _msg( "unsupported feature - " )
            {
                _msg += msg;
            }

            common::Position position() const COMPILER_NOEXCEPT override {
                return _position;
            }

            const char *what() const COMPILER_NOEXCEPT override {
                return _msg.c_str();
            }
        };
    }
}
