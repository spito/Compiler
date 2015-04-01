#pragma once

#include "Exception.h"
#include "../common/Unicode.h"

#include <string>
#include <cctype>

namespace compiler {
    namespace exception {

        class InvalidCharacter : public Exception, Given, Expected {
            std::string _given;
            std::string _expected;
            common::Position _position;
        protected:
            InvalidCharacter( std::string given, std::string expected, common::Position p ) :
                Exception( "invalid character" ),
                _given( std::move( given ) ),
                _expected( std::move( expected ) ),
                _position( std::move( p ) )
            {}
        public:
            InvalidCharacter( char given, std::string expected, common::Position p ) :
                Exception( "invalid character" ),
                _expected( std::move( expected ) ),
                _position( std::move( p ) )
            {
                if ( std::isprint( given ) )
                    _given = given;
                else {
                    char c[ 3 ] = {0,};
                    common::Unicode::fromCharToHex( given, c[ 0 ], c[ 1 ] );
                    _given = "0x";
                    _given += c;
                }
            }
            InvalidCharacter( char given, std::string expected ) :
                InvalidCharacter( given, std::move( expected ), context::position() )
            {}

            InvalidCharacter( char given, char expected, common::Position p ) :
                InvalidCharacter( given, std::string( 1, expected ), std::move( p ) )
            {}
            InvalidCharacter( char given, char expected ) :
                InvalidCharacter( given, std::string( 1, expected ), context::position() )
            {}

            InvalidCharacter( char given, common::Position p ) :
                InvalidCharacter( given, "", std::move( p ) )
            {}

            virtual const std::string &given() const COMPILER_NOEXCEPT {
                return _given;
            }
            virtual const std::string &expected() const COMPILER_NOEXCEPT {
                return _expected;
            }
            common::Position position() const COMPILER_NOEXCEPT override {
                return _position;
            }
        };
    }
}
