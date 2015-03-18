#pragma once

#include "../common/SymbolTable.h"

#include <string>
#include <memory>

namespace compiler {
    namespace preprocessor {

        struct Preprocessor {

            Preprocessor( const std::string & );


            void saveToFile( const std::string & ) const;
            size_t size() const {
                return _content.size();
            }
            const char *content() const {
                return _content.c_str();
            }

            Preprocessor( const Preprocessor & ) = delete;
            Preprocessor( Preprocessor &&other ) :
                _content( std::move( other._content ) ),
				_input( std::move( other._input ) ),
				_symbols( std::move( other._symbols ) )
            {}


        private:

            Preprocessor( const std::string &, std::shared_ptr< common::SymbolTable > );

            void processText();
            void processCommentShort();
            void processCommnetLong();
            void invokeExpression();
            void includeFile( std::string );
            void includeSystemFile( std::string );

            common::Position position() const {
                return _input.position();
            }
            void position( common::Position p ) {
                _input.position( std::move( p ) );
            }

            std::string _content;

            common::InputBuffer _input;
            std::shared_ptr< common::SymbolTable > _symbols;

        };

    }

}
