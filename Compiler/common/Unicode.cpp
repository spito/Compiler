#include "Unicode.h"
#include "../includes/exceptions.h"

#include <cctype>
#include <array>

namespace compiler {
namespace common {
namespace Unicode {

enum class Mask : uchar {
    Tail = 0x3f,
    U8 = 0x7f,
    U16 = 0x1f,
    U24 = 0x0f,
    U32 = 0x07,
    U40 = 0x03,
    U48 = 0x01
};
enum class Prefix : uchar {
    Tail = 0x80,
    U8 = 0,
    U16 = 0xc0,
    U24 = 0xe0,
    U32 = 0xf0,
    U40 = 0xf8,
    U48 = 0xfc
};

bool hex2char( char c, uchar &u ) {
    if ( std::isdigit( c ) ) {
        u = uchar( c - '0' );
        return true;
    }
    if ( std::isxdigit( c ) ) {
        u = std::islower( c ) ?
            uchar( c - 'a' + 10 ) :
            uchar( c - 'A' + 10 );
        return true;
    }
    return false;
}

char char2hex( uchar c, bool uppercase ) {
    // allow just lower 4 bits
    c &= 15;

    if ( c < 10 )
        return '0' + c;

    return ( uppercase ? 'A' : 'a' ) - 10 + c;
}

bool fromHexToChar( char higher, char lower, uchar &out ) {

    uchar hi{};
    uchar lo{};

    bool result =
        hex2char( higher, hi ) &&
        hex2char( lower, lo );
    if ( result )
        out = ( hi << 4 ) | lo;
    return result;
}

void fromCharToHex( uchar c, char &higher, char &lower, bool uppercase ) {
    uchar hi = uchar( c ) >> 4;
    uchar lo = uchar( c );

    higher = char2hex( hi, uppercase );
    lower = char2hex( lo, uppercase );
}

inline uchar modifyChar( Prefix prefix, Mask mask, uchar c ) {
    return uchar( prefix ) | ( uchar( mask ) & c );
}
inline uchar modifyChar( Mask mask, uchar c ) {
    return uchar( mask ) & c;
}

class ConversionTable {

    class Proxy {
        int _shift;
        uint32_t &_ref;
    public:
        Proxy( int shift, uint32_t &ref ) :
            _shift( shift ),
            _ref( ref )
        {}

        Proxy &operator=( uchar value ) {
            uint32_t mask = ~( uint32_t( 0xFF ) << _shift );
            _ref &= mask;
            _ref |= uint32_t( value ) << _shift;
            return *this;
        }
        Proxy &operator=( const Proxy & ) = delete;
    };

    int _base;
    uint32_t _code;
public:
    uint32_t code() const {
        return _code;
    }

    uint32_t &code() {
        return _code;
    }

    void base( int b ) {
        _base = b;
    }

    void shrinkUTF8() {
        uint32_t mask = uint32_t( Mask::Tail );
        uint32_t head = uint32_t( 3 ) << 6;
        uint32_t tailMask = uint32_t( 3 ) << 6;
        uint32_t tail = uint32_t( 1 ) << 7;
        uint32_t tailErase = ~uint32_t( 0xff );

        while ( ( _code & tailMask ) == tail ) {
            _code = ( _code & mask ) | ( ( _code & tailErase ) >> 2 );
            mask = ( mask << 6 ) | mask;
            tail <<= 6;
            tailMask <<= 6;
            tailErase <<= 6;
            head = ( ( head >> 1 ) | head ) << 6;
        }
        _code &= ~head;
    }

    Proxy operator[]( size_t index ) {
        return Proxy( 8 * ( _base - index - 1 ), _code );
    }

    ConversionTable( int base ) :
        _base( base ),
        _code( 0 )
    {}
};

bool isFirstPart( std::string unicode ) {
    uchar c;
    ConversionTable table( 2 );
    if ( unicode.size() != 4 )
        return false;

    for ( int i = 0; i < 2; ++i ) {
        fromHexToChar( unicode[ 2 * i ], unicode[ 2 * i + 1 ], c );
        table[ i ] = c;
    }
    return table.code() >= 0xD800 && table.code() <= 0xDBFF;
}

bool isSecondPart( std::string unicode ) {
    uchar c;
    ConversionTable table( 2 );
    if ( unicode.size() != 4 )
        return false;

    for ( int i = 0; i < 2; ++i ) {
        fromHexToChar( unicode[ 2 * i ], unicode[ 2 * i + 1 ], c );
        table[ i ] = c;
    }

    return table.code() >= 0xDC00 && table.code() <= 0xDFFF;
}

std::string getUTF8( uint32_t unicode ) {
    std::string result;

    if ( unicode < 0x80 )
        result += modifyChar( Prefix::U8, Mask::U8, uchar( unicode ) );
    else if ( unicode < 0x800 ) {
        result += modifyChar( Prefix::U16, Mask::U16, uchar( unicode >> 6 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode ) );
    }
    else if ( unicode < 0x10000 ) {
        result += modifyChar( Prefix::U24, Mask::U24, uchar( unicode >> 12 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode >> 6 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode ) );
    }
    else if ( unicode < 0x10FFFF ) {
        result += modifyChar( Prefix::U32, Mask::U32, uchar( unicode >> 18 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode >> 12 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode >> 6 ) );
        result += modifyChar( Prefix::Tail, Mask::Tail, uchar( unicode ) );
    }
    else
        throw exception::Unicode( "code out of range" );
    return result;
}

std::string getUTF8( const std::string &input ) {

    std::vector< uchar > hex;

    if ( input.size() % 2 )
        throw exception::Unicode( "Unsupported length of unicode character" );


    for ( auto i = input.begin(); i != input.end(); i += 2 ) {
        uchar c;
        if ( !fromHexToChar( *i, *( i + 1 ), c ) )
            throw exception::Unicode( "Invalid input format" );
        hex.push_back( c );
    }

    ConversionTable table( 2 );

    switch ( hex.size() ) {
    case 1:
        table.base( 1 );
        table[ 0 ] = hex.front();
        break;
    case 2:
        for ( int i = 0; i < 2; ++i )
            table[ i ] = hex[ i ];
        break;
    case 4:
        for ( int i = 0; i < 2; ++i )
            table[ i ] = hex[ i ];
        if ( table.code() >= 0xD800 && table.code() <= 0xDBFF ) {
            ConversionTable helper( 2 );
            for ( int i = 0; i < 2; ++i )
                helper[ i ] = hex[ i + 2 ];

            if ( helper.code() >= 0xDC00 && helper.code() <= 0xDFFF )
                table.code() = ( table.code() << 10 ) + helper.code() - 0x35FDC00;
        }
        break;
    default:
        throw exception::Unicode( "Unsupported length of unicode character" );
    }
    return getUTF8( table.code() );
}

inline bool isUnicode( uchar c ) {
    return ( uchar( Prefix::Tail ) & c ) == uchar( Prefix::Tail );
}

inline int unicodeLength( uchar c ) {
    int length = 0;
    while ( uchar( Prefix::Tail ) & c ) {
        c <<= 1;
        ++length;
    }
    return length;
}

std::string unicodeToAscii( const std::string &text, size_t &index ) {
    int length = unicodeLength( text[ index ] );
    if ( length > 4 )
        throw exception::Unicode( "Unsupported length of UTF-8 character" );

    if ( text.size() < index + length )
        length = text.size() - index;

    ConversionTable table( length );

    for ( int i = 0; i < length; ++i )
        table[ i ] = text[ index + i ];
    table.shrinkUTF8();
    index += length - 1;

    return getUnicodeCode( table.code() );
}

std::string toAscii( const std::string &text ) {
    std::string result;

    for ( size_t i = 0; i < text.size(); ++i ) {
        if ( isUnicode( text[ i ] ) )
            result += unicodeToAscii( text, i );
        else switch ( text[ i ] ) {
        case '"':
        case '\\':
            result += '\\';
            result += text[ i ];
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        default:
            if ( uchar( text[ i ] ) < 0x20 )
                result += getUnicodeCode( text[ i ] );
            else
                result += text[ i ];
            break;
        }
    }
    return result;
}

std::string escapeSpecials( const std::string &text ) {
    std::string result;
    for ( char c : text ) {
        switch ( c ) {
        case '"':
        case '\\':
            result += '\\';
            result += c;
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        default:
            if ( uchar( c ) < 0x20 )
                result += getUnicodeCode( c );
            else
                result += c;
            break;
        }
    }
    return result;
}

std::string getUnicodeCode( uint32_t code ) {

    std::array< char, 5 > hexLower = { 0 };
    std::array< char, 5 > hexHigher = { 0 };

    if ( code < 0x10000 ) {
        fromCharToHex( uchar( code >> 8 ), hexLower[ 0 ], hexLower[ 1 ] );
        fromCharToHex( uchar( code ), hexLower[ 2 ], hexLower[ 3 ] );
        return std::string( "\\u" ) + hexLower.data();
    }
    if ( code <= 0x10FFFF ) {
        uint32_t higher = ( code >> 10 ) + 0xD7C0;
        uint32_t lower = ( code & 0x3FF ) + 0xDC00;
        fromCharToHex( uchar( higher >> 8 ), hexHigher[ 0 ], hexHigher[ 1 ] );
        fromCharToHex( uchar( higher ), hexHigher[ 2 ], hexHigher[ 3 ] );
        fromCharToHex( uchar( lower >> 8 ), hexLower[ 0 ], hexLower[ 1 ] );
        fromCharToHex( uchar( lower ), hexLower[ 2 ], hexLower[ 3 ] );
        return std::string( "\\u" ) + hexHigher.data() + "\\u" + hexLower.data();
    }
    throw exception::Unicode( "invalid unicode character" );
}
}
}
}