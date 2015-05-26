#pragma once

#include "Utils.h"
#include "../includes/exceptions.h"

#include <cstdint>

namespace compiler {
namespace common {


struct Register {

    struct Type : common::Comparable, common::Orderable {

        enum class Kind {
            None,
            Int8,
            UInt8,
            Int16,
            UInt16,
            Int32,
            UInt32,
            Int64,
            UInt64,
            Pointer,
        };

        Type( Kind k ) : _length( 0 ), _signed( false ), _pointer( false ) {
            switch ( k ) {
            case Kind::None: break;
            case Kind::Int8:   _length = 1; _signed = true;  break;
            case Kind::UInt8:  _length = 1; _signed = false; break;
            case Kind::Int16:  _length = 2; _signed = true;  break;
            case Kind::UInt16: _length = 2; _signed = false; break;
            case Kind::Int32:  _length = 4; _signed = true;  break;
            case Kind::UInt32: _length = 4; _signed = false; break;
            case Kind::Int64:  _length = 8; _signed = true;  break;
            case Kind::UInt64: _length = 8; _signed = false; break;
            case Kind::Pointer: break;
            }
        }
        Type() : _length( 0 ), _signed( false ), _pointer( false ) {}
        Type( int8_t l, bool s ) : _length( l ), _signed( s ), _pointer( false ) {}
        Type( int8_t l, bool s, bool ) : _length( l ), _signed( s ), _pointer( true ) {}

        int length() const {
            return _length;
        }
        bool isSigned() const {
            return _signed;
        }
        bool isPointer() const {
            return _pointer;
        }
        Kind kind() const {
            if ( isPointer() )
                return Kind::Pointer;

            switch ( length() ) {
            case 1: return isSigned() ? Kind::Int8 : Kind::UInt8;  break;
            case 2: return isSigned() ? Kind::Int16 : Kind::UInt16; break;
            case 4: return isSigned() ? Kind::Int32 : Kind::UInt32; break;
            case 8: return isSigned() ? Kind::Int64 : Kind::UInt64; break;
            default:
                return Kind::None;
            }
        }

        Kind kindOfPointer() const {
            switch ( length() ) {
            case 1: return isSigned() ? Kind::Int8 : Kind::UInt8;  break;
            case 2: return isSigned() ? Kind::Int16 : Kind::UInt16; break;
            case 4: return isSigned() ? Kind::Int32 : Kind::UInt32; break;
            case 8: return isSigned() ? Kind::Int64 : Kind::UInt64; break;
            default:
                return Kind::None;
            }
        }

        bool operator==( const Type &another ) const {
            return
                length() == another.length() &&
                isSigned() == another.isSigned() &&
                isPointer() == another.isPointer();
        }

        bool operator<( const Type &another ) const {
            if ( length() < another.length() )
                return true;
            return isSigned() && !another.isSigned();
        }

    private:
        int8_t _length;
        bool _signed;
        bool _pointer;
    };

    Register() : _ulong( 0 ) {}
    Register( int8_t v ) : _char( v ), _type( 1, true ) {}
    Register( uint8_t v ) : _uchar( v ), _type( 1, false ) {}
    Register( int16_t v ) : _short( v ), _type( 2, true ) {}
    Register( uint16_t v ) : _ushort( v ), _type( 2, false ) {}
    Register( int32_t v ) : _int( v ), _type( 4, true ) {}
    Register( uint32_t v ) : _uint( v ), _type( 4, false ) {}
    Register( int64_t v ) : _long( v ), _type( 8, true ) {}
    Register( uint64_t v ) : _ulong( v ), _type( 8, false ) {}
    Register( void *v, int8_t l, bool s ) : _type( l, s, true ) {
        _ptr.ptr = v;
    }

    template< typename T >
    Register( T *v ) : _type( sizeof( T ), std::is_signed< T >::value, true ) {
        _ptr.ptr = v;
    }

    Register &operator=( Register r ) {
        r.clearMess();
        _ulong = r._ulong;
        _type = r._type;
        clearFlags();
        return *this;
    }

    void clear() {
        getu64() ^= getu64();
    }

    void quiet( bool v = true ) {
        _quiet = v;
    }

    void clearFlags() {
        _pointerProblem = false;
        _signedProblem = false;
    }

    bool isPointerProblem() const {
        return _pointerProblem;
    }
    bool isSignedProblem() const {
        return _signedProblem;
    }

    int8_t &get8() {
        return _char;
    }
    uint8_t &getu8() {
        return _uchar;
    }
    int8_t get8() const {
        return _char;
    }
    uint8_t getu8() const {
        return _uchar;
    }
    void set8( int8_t v ) {
        clear();
        _type = Type( 1, true );
        _char = v;
    }
    void setu8( uint8_t v ) {
        clear();
        _type = Type( 1, false );
        _uchar = v;
    }

    int16_t &get16() {
        return _short;
    }
    uint16_t &getu16() {
        return _ushort;
    }
    int16_t get16() const {
        return _short;
    }
    uint16_t getu16() const {
        return _ushort;
    }
    void set16( int16_t v ) {
        clear();
        _type = Type( 2, true );
        _short = v;
    }
    void setu16( uint16_t v ) {
        clear();
        _type = Type( 2, false );
        _ushort = v;
    }


    int32_t &get32() {
        return _int;
    }
    uint32_t &getu32() {
        return _uint;
    }
    int32_t get32() const {
        return _int;
    }
    uint32_t getu32() const {
        return _uint;
    }
    void set32( int32_t v ) {
        clear();
        _type = Type( 4, true );
        _int = v;
    }
    void setu32( uint32_t v ) {
        clear();
        _type = Type( 4, false );
        _uint = v;
    }

    int64_t &get64() {
        return _long;
    }
    uint64_t &getu64() {
        return _ulong;
    }
    int64_t get64() const {
        return _long;
    }
    uint64_t getu64() const {
        return _ulong;
    }
    void set64( int64_t v ) {
        clear();
        _type = Type( 8, true );
        _long = v;
    }
    void setu64( uint64_t v ) {
        clear();
        _type = Type( 8, false );
        _ulong = v;
    }

    void *&getPtr() {
        return _ptr.ptr;
    }
    void *getPtr() const {
        return _ptr.ptr;
    }
    void setPtr( void *v ) {
        clear();
        _ptr.ptr = v;
    }

    uintptr_t &getPtrUNumeric() {
        return _ptr.unumeric;
    }
    uintptr_t getPtrUNumeric() const {
        return _ptr.unumeric;
    }
    void setPtrUNumeric( uintptr_t v ) {
        clear();
        _ptr.unumeric = v;
    }
    intptr_t &getPtrNumeric() {
        return _ptr.numeric;
    }
    intptr_t getPtrNumeric() const {
        return _ptr.numeric;
    }
    void setPtrUNumeric( intptr_t v ) {
        clear();
        _ptr.numeric = v;
    }

    Type type() const {
        return _type;
    }

    bool zero() const {
        if ( type().isPointer() )
            return getPtr() == nullptr;

        switch ( type().kind() ) {
        case Type::Kind::Int8:
        case Type::Kind::UInt8:  return getu8() == 0;
        case Type::Kind::Int16:
        case Type::Kind::UInt16: return getu16() == 0;
        case Type::Kind::Int32:
        case Type::Kind::UInt32: return getu32() == 0;
        case Type::Kind::Int64:
        case Type::Kind::UInt64: return getu64() == 0;
        default:
            return false;
        }
    }

    void clearMess() {
        switch ( type().kind() ) {
        case Type::Kind::Int8:
        case Type::Kind::UInt8:  setu8( getu8() );   break;
        case Type::Kind::Int16:
        case Type::Kind::UInt16: setu16( getu16() ); break;
        case Type::Kind::Int32:
        case Type::Kind::UInt32: setu32( getu32() ); break;
        default:
        case Type::Kind::Int64:
        case Type::Kind::UInt64: setu64( getu64() ); break;
        }
    }

    void castTo( bool pointer, int length, bool sign ) {
        clearMess();
        _type = pointer ?
            Type( length, sign, true ) :
            Type( length, sign );
    }

    Register operator++( );
    Register operator--( );
    Register operator~( );
    Register operator-( );
    Register operator+( );
    Register operator!( );

    Register operator*=( Register );
    Register operator/=( Register );
    Register operator%=( Register );
    Register operator+=( Register );
    Register operator-=( Register );
    Register operator<<=( Register );
    Register operator>>=( Register );
    Register operator<( Register );
    Register operator<=( Register );
    Register operator>( Register );
    Register operator>=( Register );
    Register operator==( Register );
    Register operator!=( Register );
    Register operator&=( Register );
    Register operator^=( Register );
    Register operator|=( Register );

private:

    void promote() {
        switch ( type().kind() ) {
        case Type::Kind::Int8:   get32() = get8();    break;
        case Type::Kind::UInt8:  getu32() = getu8();  break;
        case Type::Kind::Int16:  get32() = get16();   break;
        case Type::Kind::UInt16: getu32() = getu16(); break;
        default:
            return;
        }

        type( Type( 4, type().isSigned() ) );
    }

    // http://en.cppreference.com/w/c/language/conversion#Usual_arithmetic_conversions
    void arithmeticsConversion( Register &another ) {
        promote();
        another.promote();
        // int int -> int int
        // uint uint -> uint uint
        // long long -> long long
        // ulong ulong -> ulong ulong
        if ( type() == another.type() )
            return;

        // int long -> long long
        // uint ulong -> ulong ulong
        if ( type().isSigned() == another.type().isSigned() ) {
            type().length() < another.type().length() ?
                *this = Register( uint64_t( getu32() ) ) :
                another = Register( uint64_t( another.getu32() ) );
            return;
        }

        // int uint -> uint uint
        // int ulong -> ulong ulong
        // uint long -> long long
        // long ulong -> ulong ulong
        partialConversion( max( *this, another ), min( *this, another ) );
    }

    static Register &max( Register &lhs, Register &rhs ) {
        return lhs.type() < rhs.type() ? rhs : lhs;
    }
    static Register &min( Register &lhs, Register &rhs ) {
        return lhs.type() < rhs.type() ? lhs : rhs;
    }

    static void partialConversion( Register desired, Register &toChange ) {
        // long -> ulong
        // uint -> long
        switch ( toChange.type().kind() ) {
        case Type::Kind::Int64: toChange = Register( uint64_t( toChange.get64() ) ); return;
        case Type::Kind::UInt32:toChange = Register( int64_t( toChange.getu32() ) ); return;
        default:;
        }

        // int -> uint
        // int -> ulong
        switch ( desired.type().kind() ) {
        case Type::Kind::UInt32: toChange = Register( uint32_t( toChange.get32() ) ); return;
        case Type::Kind::UInt64: toChange = Register( uint64_t( toChange.get32() ) ); return;
        default:;
        }
    }

    void type( Type t ) {
        _type = t;
    }

    Register raisePointerProblem() {
        _pointerProblem = true;
        if ( !_quiet )
            throw exception::InternalError( "register: pointer problem" );
        return *this;
    }
    Register raiseSignedProblem() {
        _signedProblem = true;
        if ( !_quiet )
            throw exception::InternalError( "register: signed problem" );
        return *this;
    }

    union Pointer {
        void *ptr;
        uintptr_t unumeric;
        intptr_t numeric;
    };

    union {

        int8_t _char;
        uint8_t _uchar;
        int16_t _short;
        uint16_t _ushort;
        int32_t _int;
        uint32_t _uint;
        int64_t _long;
        uint64_t _ulong;
        Pointer _ptr;
    };

    Type _type;
    bool _quiet;
    bool _pointerProblem = false;
    bool _signedProblem = false;
};


} // namespace common
} // namespace compiler
