#pragma once

#include "../common/TokenType.h"

#include <map>
#if 0
namespace compiler {
    namespace preprocessor {

        enum class TokenTypes {
            eof,//
            LongComment,
            ShortComment,
            Sharp,//
            TwoSharps,//
            If,
            Elif,
            Else,
            Ifdef,
            Ifndef,
            Defined,
            Define,
            Undef,
            Include,
            Pragma,
            LocalFile,
            SystemFile,
            Not,//
            And,
            Or,
            BraceOpen,//
            BraceClose,//
            Comma,//
            Text,
            InterrestingWord,
            Param,
            Integer,
            Real
        };

        extern std::map< TokenTypes, common::TokenType > tokens;

    } // namespace preprocessor
} // namespace compiler
#endif
