#pragma once

#include "InvalidCharacter.h"

namespace compiler {
    namespace exception {
    
        class EndOfFile : public InvalidCharacter {
        public:
            EndOfFile( std::string expected, common::Position p ) :
                InvalidCharacter( "EOF", std::move( expected ), std::move( p ) )
            {}
        };
    }
}
