#include "Preprocessor.h"
#include "../common/InputBuffer.h"

#include <fstream>

namespace compiler {
    namespace preprocessor {

        void Preprocessor::saveToFile( const std::string &name ) const {
            std::ofstream out( name.c_str() );

            out << content();
        }


        Preprocessor::Preprocessor( const std::string &name ) {
            _input.openFile( name.c_str() );
        }


        void Preprocessor::processText() {
            common::Position before;
            bool readyForCommand = true;
            bool doubleQuotes = false;
            bool singleQuotes = false;
            bool slash = false;

            while ( !_input.eof() ) {
                before = position();
                char c = _input.read();

                if ( c == '#' ) {
                    if ( singleQuotes || doubleQuotes ) {
                        _content += '#';
                        continue;
                    }
                    if ( readyForCommand ) {
                        invokeExpression();
                        continue;
                    }
                    throw exception::InvalidCharacter( '#', before );
                }
                if ( c == '/' ) {
                    common::Position wannaBack = position();
                    switch ( _input.read() ) {
                    case '/':
                        processCommentShort();
                        readyForCommand = true;
                        continue;
                    case '*':
                        processCommnetLong();
                        continue;
                    default:
                        position( wannaBack );
                    }
                }

                if ( slash ) {
                    _content += c;
                    slash = false;
                    continue;
                }

                if ( c == '\\' )
                    slash = true;
                else if ( c == '\n' )
                    readyForCommand = true;

                if ( !std::isspace( c ) )
                    readyForCommand = false;

                _content += c;
            }
        }

        void Preprocessor::processCommentShort() {
            bool slash = false;
            while ( !_input.eof() ) {
                char c = _input.read();
                if ( slash ) {
                    slash = false;
                    continue;
                }

                if ( c == '\\' )
                    slash = true;
                if ( c == '\n' )
                    break;
            }
        }

        void Preprocessor::processCommnetLong() {
            bool star = false;
            while ( !_input.eof() ) {
                char c = _input.read();
                if ( c == '*' )
                    star = true;
                else if ( star && c == '/' )
                    break;
                else
                    star = false;
            }
        }

		void Preprocessor::invokeExpression()
		{
		}

        void Preprocessor::includeFile( std::string )
        {
        }

        void Preprocessor::includeSystemFile( std::string )
        {
        }


    } // namespace preprocessor
} // namespace compiler


