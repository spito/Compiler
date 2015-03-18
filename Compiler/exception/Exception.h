#pragma once

#include "../common/Position.h"

#include <stdexcept>

#ifdef _MSC_VER
#define COMPILER_NOEXCEPT throw()
#else
#define COMPILER_NOEXCEPT noexcept
#endif

namespace compiler {
    namespace exception {

        class Exception : public std::runtime_error {
        protected:
            Exception( const char *msg ) :
                std::runtime_error( msg )
            {}
        public:
            virtual ~Exception(){}
            virtual common::Position position() const COMPILER_NOEXCEPT = 0;
        };

        class Given {
        public:
            virtual ~Given() = default;
            virtual const std::string &given() const COMPILER_NOEXCEPT = 0;
        };

        class Expected {
        public:
            virtual ~Expected() = default;
            virtual const std::string &expected() const COMPILER_NOEXCEPT = 0;
        };
    }
}
