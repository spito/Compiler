#pragma once

#include "Exception.h"
#include "../common/Token.h"

#include <vector>

namespace compiler {
    namespace exception {

        class InvalidToken : public Exception {
            common::Token _given;
            std::vector< common::TokenType * > _expected;
        public:
            InvalidToken( common::Token given, std::vector< common::TokenType * > expected ) :
                Exception( "invalid token" ),
                _given( std::move( given ) ),
                _expected( std::move( expected ) )
            {}

            InvalidToken( common::Token given, common::TokenType *expected ) :
                InvalidToken( std::move( given ), std::vector< common::TokenType * >() )
            {
                _expected.emplace_back( std::move( expected ) );
            }

            const common::Token &given() const COMPILER_NOEXCEPT {
                return _given;
            }

            common::Position position() const COMPILER_NOEXCEPT override{
                return _given.position();
            }

            const std::vector< common::TokenType * > &expected() const COMPILER_NOEXCEPT {
                return _expected;
            }
        };
    }
}
