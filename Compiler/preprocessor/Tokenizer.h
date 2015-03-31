#pragma once

#include "Tokens.h"
#include "../common/InputBuffer.h"

namespace compiler {
    namespace preprocessor {

        struct Tokenizer {

            enum class Class {
                None,
                Slash,
                Number,
                Word,
                Space,
                String,
                ShortComment,
                LongComment,
                SingleOperator,
                CompoundOperator,
                Preprocessor,
            };

            Tokenizer( std::ifstream &file ) :
                _input( file )
            {}

            Tokenizer( const Tokenizer & ) = delete;
            Tokenizer( Tokenizer &&other ) :
                _input( std::move( other._input ) )
            {}

            void restart( const std::string &content ) {
                _input.assignContent( content );
            }

            bool isTokenType( common::Token &, const common::Token::Type );

            // destroying
            void readToken( common::Token &, Class = Class::None );

            // not-destroying
            void lookAtToken( common::Token &, Class = Class::None );

            common::Position position() const {
                return _input.position();
            }
            void position( common::Position p ) {
                _input.position( std::move( p ) );
            }

        private:

            Class resolveClass( Class = Class::None );

            void getToken( common::Token &, Class = Class::None );

            void processNumber( common::Token & );
            void processWord( common::Token & );
            void processSpace( common::Token & );
            void processString( common::Token & );
            void processShortComment( common::Token & );
            void processLongComment( common::Token & );
            void processSingleOperator( common::Token & );
            bool processCompoundOperator( common::Token & );
            void processSlash( common::Token & );
            void processSharp( common::Token & );

            common::InputBuffer _input;
            bool _readyForCommand = true;

        };

    } // namespace preprocessor
} // namespace compiler
