#include "Tokens.h"
#if 0
namespace compiler {
    namespace preprocessor {

#define EXPLICIT( k, v ) { TokenTypes:: k, { #k, v } }
#define SYMBOLIC( k ) { TokenTypes:: k, { #k } }


        std::map< TokenTypes, common::TokenType > tokens = {
            SYMBOLIC( eof ),
            SYMBOLIC( LongComment ),
            SYMBOLIC( ShortComment ),
            EXPLICIT( Sharp, "#" ),
            EXPLICIT( TwoSharps, "##" ),
            EXPLICIT( If, "if" ),
            EXPLICIT( Elif, "elif" ),
            EXPLICIT( Else, "else" ),
            EXPLICIT( Ifdef, "ifdef" ),
            EXPLICIT( Ifndef, "ifndef" ),
            EXPLICIT( Defined, "defined" ),
            EXPLICIT( Define, "define" ),
            EXPLICIT( Undef, "undef" ),
            EXPLICIT( Include, "include" ),
            EXPLICIT( Pragma, "pragma" ),
            SYMBOLIC( LocalFile ),
            SYMBOLIC( SystemFile ),
            EXPLICIT( Not, "!" ),
            EXPLICIT( And, "&&" ),
            EXPLICIT( Or, "||" ),
            EXPLICIT( BraceOpen, "(" ),
            EXPLICIT( BraceClose, ")" ),
            EXPLICIT( Comma, "," ),
            SYMBOLIC( Text ),
            SYMBOLIC( InterrestingWord ),
            SYMBOLIC( Param ),
            EXPLICIT( Integer, common::Numeric::Integral ),
            EXPLICIT( Real, common::Numeric::Real )
        };

    } // namespace preprocessor
} // namespace compiler
#endif