#pragma once

#include "Exception.h"

namespace compiler {
    namespace exception {

        class InvalidFile : public Exception {
        public:
            InvalidFile() :
                Exception( "invalid input file" )
            {}

            virtual common::Position position() const COMPILER_NOEXCEPT override {
                return common::Position::nowhere();
            }

        };
    }
}
