#pragma once

namespace compiler {
namespace common {

bool isNumber( int c );
bool isWordBegin( int c );
bool isWordLater( int c );
bool isSpace( int c );
bool isString( int c );
bool isShortComment( int c1, int c2 );
bool isLongComment( int c1, int c2 );
bool isSingleOperator( int c );
bool isCompoundOperatorBegin( int c );
bool isCompounfOperatorLater( int c );
bool isPreprocessorSign( int c );
bool isSlash( int c );
bool isNewLine( int c );

bool isSpace( const char * );
bool isWord( const char * );

} // namespace common
} // namespace compiler
