#pragma once

#include "Exception.h"
#include "../common/Symbol.h"

namespace compiler {
    namespace exception {

        class DuplicateSymbol : public Exception {
            common::Symbol _symbol;
            common::Position _position;
        public:
            DuplicateSymbol( common::Symbol symbol, common::Position p ) :
                Exception( "duplicate symbol" ),
                _symbol( std::move( symbol ) ),
                _position( std::move( p ) )
            {}
            DuplicateSymbol( common::Symbol symbol ) :
                Exception( "duplicate symbol" ),
                _symbol( std::move( symbol ) ),
                _position( context::position() )
            {}

            common::Position position() const COMPILER_NOEXCEPT override{
                return _position;
            }
        };

    } // namespace exception
} // namespace compiler