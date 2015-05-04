#pragma once

#include "Core.h"
#include "../common/Utils.h"
#include <cstdint>

namespace compiler {
namespace ast {
namespace type {

enum class Kind {
    Elementary,
    Array,
    Pointer,
};

struct Type : DynamicCast, common::Comparable {
    Type( Kind kind ) :
        _kind( kind )
    {}
    Kind kind() const {
        return _kind;
    }
    bool operator==( const Type &other ) const {
        return equal( &other );
    }
    bool equal( const Type *other ) const {
        if ( kind() != other->kind() )
            return false;
        return equalInherited( other );
    }
    virtual size_t hash() const = 0;
    virtual size_t size() const = 0;
private:
    virtual bool equalInherited( const Type * ) const = 0;
    Kind _kind;
};

struct Elementary : Type {

    Elementary( int8_t length, bool s, bool c = false ) :
        Type( Kind::Elementary ),
        _length( length ),
        _signed( s ),
        _const( c )
    {}

    size_t size() const override {
        return _length;
    }
    int length() const {
        return _length;
    }
    bool isSigned() const {
        return _signed;
    }
    bool isConst() const {
        return _const;
    }
    size_t hash() const override {
        return
            size_t( length() ) << 2 |
            size_t( isSigned() ) << 1 |
            size_t( isConst() );
    }
private:
    bool equalInherited( const Type *_other ) const override {
        const Elementary *other = _other->as< Elementary >();
        return
            length() == other->length() &&
            isSigned() == other->isSigned() &&
            isConst() == other->isConst();
    }

    int8_t _length;
    bool _signed;
    bool _const;
};

struct Array : Type {
    Array( const Type *of, size_t count ) :
        Type( Kind::Array ),
        _of( *of ),
        _count( count )
    {}

    size_t size() const override {
        return _count * of().size();
    }
    const Type &of() const {
        return _of;
    }
    size_t count() const {
        return _count;
    }
    size_t hash() const override {
        return
            of().hash() << 4 |
            size();
    }
private:
    bool equalInherited( const Type *_other ) const override {
        const Array *other = _other->as< Array >();
        return
            size() == other->size() &&
            of() == other->of();
    }
    const Type &_of;
    size_t _count;
};

struct Pointer : Type {

    Pointer( const Type *of, bool c = false ) :
        Type( Kind::Pointer ),
        _of( *of ),
        _const( c )
    {}

    size_t size() const override {
        return sizeof( void * );
    }
    const Type &of() const {
        return _of;
    }
    bool isConst() const {
        return _const;
    }
    size_t hash() const {
        return
            of().hash() << 1 |
            size_t( isConst() );
    }
private:
    bool equalInherited( const Type *_other ) const override {
        const Pointer *other = _other->as< Pointer >();
        return
            isConst() == other->isConst() &&
            of() == other->of();
    }
    const Type &_of;
    bool _const;
};

} // namespace type
} // namespace ast
} // namespace compiler
