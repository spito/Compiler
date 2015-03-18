#pragma once

#include "Exception.h"
#include <string>

namespace compiler {
    namespace exception {

        class Unicode : public Exception {
            std::string _msg;
        protected:
            Unicode( const char *prefix, const char *msg ) :
                Exception( "" ),
                _msg( prefix )
            {
                _msg += msg;
            }
        public:
            Unicode( const char *msg ) :
                Exception( "" ),
                _msg( msg )
            {}

            const char *what() const COMPILER_NOEXCEPT override{
                return _msg.c_str();
            }
            common::Position position() const COMPILER_NOEXCEPT override{
                return common::Position::nowhere();
            }

        };

        class UnicodeEncoding : public Unicode {
            common::Position _position;
        public:
            UnicodeEncoding( const Unicode &ex, common::Position p ) :
                Unicode( "Unicode encoding error - ", ex.what() ),
                _position( std::move( p ) )
            {}

            UnicodeEncoding( const char *msg, common::Position p ) :
                Unicode( "Unicode encoding error - ", msg ),
                _position( std::move( p ) )
            {}

            common::Position position() const COMPILER_NOEXCEPT override{
                return _position;
            }
        };

        class UnicodeDecoding : public Unicode {
            common::Position _position;
        public:
            UnicodeDecoding( const Unicode &ex, common::Position p ) :
                Unicode( "Unicode decoding error - ", ex.what() ),
                _position( std::move( p ) )
            {}

            common::Position position() const COMPILER_NOEXCEPT override{
                return _position;
            }
        };
    }
}