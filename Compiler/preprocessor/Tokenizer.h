#pragma once

#include "Tokens.h"
#include "../common/InputBuffer.h"

namespace compiler {
    namespace preprocessor {

        struct Tokenizer {
            
            Tokenizer( std::ifstream &file ) :
                _input( file )
            {}

            bool isTokenType( common::Token &, const common::Token::Type );

            // destroying
            void readToken( common::Token & );

            // not-destroying
            void lookAtToken( common::Token & );

            common::Position position() const {
                return _input.position();
            }

        private:

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
                PreprocessorTwice
            };

            void position( common::Position p ) {
                _input.position( std::move( p ) );
            }
            Class resolveClass( Class = Class::None);

            void getToken( common::Token & );

            void processNumber( common::Token & );
            void processWord( common::Token & );
            void processSpace( common::Token & );
            void processString( common::Token & );
            void processShortComment( common::Token & );
            void processLongComment( common::Token & );
            void processSingleOperator( common::Token & );
            bool processCompoundOperator( common::Token & );

            common::InputBuffer _input;
            bool _readyForCommand = true;

        };

    } // namespace preprocessor
} // namespace compiler
