#pragma once

#include "Exception.h"

namespace compiler {
    namespace exception {

        class InvalidCharacterConstant : public Exception, Given {
            std::string _given;
            common::Position _position;
        public:
            InvalidCharacterConstant( std::string given, common::Position position ) :
                Exception( "invalid character constant" ),
                _given( given ),
                _position( position )
            {}

            common::Position position() const COMPILER_NOEXCEPT override {
                return _position;

            }
            const std::string &given() const COMPILER_NOEXCEPT override {
                return _given;
            }

        };

    } // namespace exception
} // namespace compiler

