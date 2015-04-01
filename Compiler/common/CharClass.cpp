#include "CharClass.h"

namespace compiler {
namespace common {

enum {
    NOTHING,
    NUMBER = 0x1,
    WORD_BEGIN = 0x2,
    SPACE = 0x4,
    STRING = 0x8,
    SHORT_COMMENT_1 = 0x10,
    SHORT_COMMENT_2 = 0x20,
    LONG_COMMENT_1 = 0x40,
    LONG_COMMENT_2 = 0x80,
    OPERATOR = 0x100,
    PREPROCESSOR_SIGN = 0x200,
    WORD_LATER = 0x400,
    SLASH = 0x800,
    NEW_LINE = 0x1000
};

static const int characters[] = {
    NOTHING, // 0x00 Null char
    NOTHING, // 0x01 Start of Heading
    NOTHING, // 0x02 Start of Text
    NOTHING, // 0x03 End of Text
    NOTHING, // 0x04 End of Transmission
    NOTHING, // 0x05 Enquiry
    NOTHING, // 0x06 Acknowledgment
    NOTHING, // 0x07 Bell
    NOTHING, // 0x08 Back Space
    SPACE,   // 0x09 Horizontal Tab \t
    SPACE | NEW_LINE,   // 0x0A Line Feed \n
    NOTHING, // 0x0B Vertical Tab
    NOTHING, // 0x0C Form Feed
    SPACE,   // 0x0D Carriage Return \r
    NOTHING, // 0x0E Shift Out
    NOTHING, // 0x0F Shift In
    NOTHING, // 0x10 Data Line Escape
    NOTHING, // 0x11 Device Control 1
    NOTHING, // 0x12 Device Control 2
    NOTHING, // 0x13 Device Control 3
    NOTHING, // 0x14 Device Control 4
    NOTHING, // 0x15 Negatice Acknowledgment
    NOTHING, // 0x16 Synchonous Idle
    NOTHING, // 0x17 End of Transmith Block
    NOTHING, // 0x18 Cancel
    NOTHING, // 0x19 End of Medium
    NOTHING, // 0x1A Substitute
    NOTHING, // 0x1B Escape
    NOTHING, // 0x1C File Separator
    NOTHING, // 0x1D Group Separator
    NOTHING, // 0x1E Record Separator
    NOTHING, // 0x1F Unit Separator
    SPACE, // space
    OPERATOR, // !
    STRING, // "
    PREPROCESSOR_SIGN, // #
    NOTHING, // $
    OPERATOR, // %
    OPERATOR, // &
    STRING, // '
    OPERATOR, // (
    OPERATOR, // )
    OPERATOR | LONG_COMMENT_2, // *
    OPERATOR, // +
    OPERATOR, // ,
    OPERATOR | NUMBER, // -
    OPERATOR, // .
    OPERATOR | SHORT_COMMENT_1 | SHORT_COMMENT_2 | LONG_COMMENT_1, // /
    NUMBER | WORD_LATER, // 0
    NUMBER | WORD_LATER, // 1
    NUMBER | WORD_LATER, // 2
    NUMBER | WORD_LATER, // 3
    NUMBER | WORD_LATER, // 4
    NUMBER | WORD_LATER, // 5
    NUMBER | WORD_LATER, // 6
    NUMBER | WORD_LATER, // 7
    NUMBER | WORD_LATER, // 8
    NUMBER | WORD_LATER, // 9
    OPERATOR, // :
    OPERATOR, // ;
    STRING | OPERATOR, // <
    OPERATOR, // =
    OPERATOR, // >
    OPERATOR, // ?
    NOTHING, // @
    WORD_BEGIN | WORD_LATER, // A
    WORD_BEGIN | WORD_LATER, // B
    WORD_BEGIN | WORD_LATER, // C
    WORD_BEGIN | WORD_LATER, // D
    WORD_BEGIN | WORD_LATER, // E
    WORD_BEGIN | WORD_LATER, // F
    WORD_BEGIN | WORD_LATER, // G
    WORD_BEGIN | WORD_LATER, // H
    WORD_BEGIN | WORD_LATER, // I
    WORD_BEGIN | WORD_LATER, // J
    WORD_BEGIN | WORD_LATER, // K
    WORD_BEGIN | WORD_LATER, // L
    WORD_BEGIN | WORD_LATER, // M
    WORD_BEGIN | WORD_LATER, // N
    WORD_BEGIN | WORD_LATER, // O
    WORD_BEGIN | WORD_LATER, // P
    WORD_BEGIN | WORD_LATER, // Q
    WORD_BEGIN | WORD_LATER, // R
    WORD_BEGIN | WORD_LATER, // S
    WORD_BEGIN | WORD_LATER, // T
    WORD_BEGIN | WORD_LATER, // U
    WORD_BEGIN | WORD_LATER, // V
    WORD_BEGIN | WORD_LATER, // W
    WORD_BEGIN | WORD_LATER, // X
    WORD_BEGIN | WORD_LATER, // Y
    WORD_BEGIN | WORD_LATER, // Z
    OPERATOR, // [
    SLASH, // \ 
    OPERATOR, // ]
    OPERATOR, // ^
    WORD_BEGIN | WORD_LATER, // _
    NOTHING, // `
    WORD_BEGIN | WORD_LATER, // a
    WORD_BEGIN | WORD_LATER, // b
    WORD_BEGIN | WORD_LATER, // c
    WORD_BEGIN | WORD_LATER, // d
    WORD_BEGIN | WORD_LATER, // e
    WORD_BEGIN | WORD_LATER, // f
    WORD_BEGIN | WORD_LATER, // g
    WORD_BEGIN | WORD_LATER, // h
    WORD_BEGIN | WORD_LATER, // j
    WORD_BEGIN | WORD_LATER, // k
    WORD_BEGIN | WORD_LATER, // l
    WORD_BEGIN | WORD_LATER, // m
    WORD_BEGIN | WORD_LATER, // n
    WORD_BEGIN | WORD_LATER, // o
    WORD_BEGIN | WORD_LATER, // p
    WORD_BEGIN | WORD_LATER, // q
    WORD_BEGIN | WORD_LATER, // r
    WORD_BEGIN | WORD_LATER, // s
    WORD_BEGIN | WORD_LATER, // t
    WORD_BEGIN | WORD_LATER, // u
    WORD_BEGIN | WORD_LATER, // v
    WORD_BEGIN | WORD_LATER, // w
    WORD_BEGIN | WORD_LATER, // x
    WORD_BEGIN | WORD_LATER, // y
    WORD_BEGIN | WORD_LATER, // z
    OPERATOR, // {
    OPERATOR, // |
    OPERATOR, // }
    OPERATOR, // ~
    NOTHING // 0x7F Delete
};

static bool isCharClass( int c, int charClass ) {
    return unsigned( c ) < sizeof( characters ) / sizeof( *characters ) &&
        characters[ c ] & charClass;
}

bool isNumber( int c ) {
    return isCharClass( c, NUMBER );
}
bool isWordBegin( int c ) {
    return isCharClass( c, WORD_BEGIN );
}
bool isWordLater( int c ) {
    return isCharClass( c, WORD_LATER );
}
bool isSpace( int c ) {
    return isCharClass( c, SPACE );
}
bool isString( int c ) {
    return isCharClass( c, STRING );
}
bool isShortComment( int c1, int c2 ) {
    return
        isCharClass( c1, SHORT_COMMENT_1 ) &&
        isCharClass( c2, SHORT_COMMENT_2 );
}
bool isLongComment( int c1, int c2 ) {
    return
        isCharClass( c1, LONG_COMMENT_1 ) &&
        isCharClass( c2, LONG_COMMENT_2 );
}
bool isOperator( int c ) {
    return isCharClass( c, OPERATOR );
}
bool isPreprocessorSign( int c ) {
    return isCharClass( c, PREPROCESSOR_SIGN );
}
bool isSlash( int c ) {
    return isCharClass( c, SLASH );
}
bool isNewLine( int c ) {
    return isCharClass( c, NEW_LINE );
}

bool isSpace( const char *str ) {

    for ( ; *str; ++str )
        if ( !isSpace( *str ) )
            return false;
    return true;
}

bool isWord( const char *str ) {
    if ( !isWordBegin( *str ) )
        return false;
    for ( ++str; *str; ++str )
        if ( !isWordLater( *str ) )
            return false;
    return true;
}

} // namespace commmon
} // namespace compiler
