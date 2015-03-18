#pragma once

#include "Exception.h"

#include <string>

namespace compiler {
    namespace exception {

        class InternalError : public Exception {
            std::string _msg;
            common::Position _position;
        public:
            InternalError( const std::string &msg, common::Position p ) :
                Exception( "" ),
                _msg( "internal error - " + msg ),
                _position( std::move( p ) )
            {}

            const char *what() const COMPILER_NOEXCEPT override{
                return _msg.c_str();
            }

            common::Position position() const COMPILER_NOEXCEPT override{
                return _position;
            }
        };
    }
}
