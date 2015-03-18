#pragma once

#include "Exception.h"
#include "../common/Symbol.h"

namespace compiler {
    namespace exception {

        class DuplicateSymbol : public Exception {
            common::SymbolPtr _old;
            common::SymbolPtr _new;
            common::Position _position;
        public:
            DuplicateSymbol( common::SymbolPtr oldS, common::SymbolPtr newS, common::Position p ) :
                Exception( "duplicate symbol" ),
                _old( oldS ),
                _new( newS ),
                _position( std::move( p ) )
            {}

            common::Position position() const COMPILER_NOEXCEPT override{
                return _position;
            }
        };

    } // namespace exception
} // namespace compiler