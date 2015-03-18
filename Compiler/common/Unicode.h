#pragma once

#include <string>
#include <stdint.h>

namespace compiler {
    namespace common {
        namespace Unicode {
            using uchar = unsigned char;

            // This functions can encode multi-byte character
            // from \uXXXX into UTF-8 encoding
            // Supports length of 2, 4 and 8
            std::string getUTF8( const std::string & );

            // This function can decode whole text into \uXXXX escape sequence
            // If useExtension is set to true,
            // the function is allowed to generate \xXX escape sequences
            // if it is reliable
            std::string toAscii( const std::string & );
            std::string escapeSpecials( const std::string & );

            // Return true if \uXXXX is upper part of UTF-8 long character
            // Assume 4 bytes long string.
            bool isFirstPart( std::string );
            bool isSecondPart( std::string );

            // Create character from hexadecimal pair of values
            // If the input values are correct, the function returns
            // true and the caracter is written into `out` variable.
            // Otherwise false is returned and nothing is written
            // into `out` variable.
            bool fromHexToChar( char higher, char lower, uchar &out );


            void fromCharToHex( uchar c, char &higher, char &lower, bool uppercase = true );

            // decode Unicode character into \uXXXX escape sequence
            // Support characters in range 0 - 0x10FFFF
            // (standard UTF-8 and UTF-16 range)
            std::string getUnicodeCode( uint32_t );

        };
    }
}
