#include "Register.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace common {

Register Register::operator++( ) {
    switch ( type().kind() ) {
    case Type::Kind::None:               break;
    case Type::Kind::Int8:   ++get8();   break;
    case Type::Kind::UInt8:  ++getu8();  break;
    case Type::Kind::Int16:  ++get16();  break;
    case Type::Kind::UInt16: ++getu16(); break;
    case Type::Kind::Int32:  ++get32();  break;
    case Type::Kind::UInt32: ++getu32(); break;
    case Type::Kind::Int64:  ++get64();  break;
    case Type::Kind::UInt64: ++getu64(); break;
    case Type::Kind::Pointer: getPtrUNumeric() += type().length(); break;
    }
    return *this;
}

Register Register::operator--( ) {
    switch ( type().kind() ) {
    case Type::Kind::None:               break;
    case Type::Kind::Int8:   --get8();   break;
    case Type::Kind::UInt8:  --getu8();  break;
    case Type::Kind::Int16:  --get16();  break;
    case Type::Kind::UInt16: --getu16(); break;
    case Type::Kind::Int32:  --get32();  break;
    case Type::Kind::UInt32: --getu32(); break;
    case Type::Kind::Int64:  --get64();  break;
    case Type::Kind::UInt64: --getu64(); break;
    case Type::Kind::Pointer: getPtrUNumeric() -= type().length(); break;
    }
    return *this;
}

Register Register::operator~( ) {
    switch ( type().kind() ) {
    case Type::Kind::None:               break;
    case Type::Kind::Int8:
    case Type::Kind::Int16:
    case Type::Kind::Int32:
    case Type::Kind::Int64: return raiseSignedProblem();
    case Type::Kind::UInt8:  setu8( ~getu8() );  break;
    case Type::Kind::UInt16: setu16( ~getu16() ); break;
    case Type::Kind::UInt32: setu32( ~getu32() ); break;
    case Type::Kind::UInt64: setu64( ~getu64() ); break;
    case Type::Kind::Pointer: return raisePointerProblem();
    }
    return *this;
}

Register Register::operator-( ) {
    switch ( type().kind() ) {
    case Type::Kind::None:               break;
    case Type::Kind::Int8:  set32( -get8() );  break;
    case Type::Kind::Int16: set32( -get16() ); break;
    case Type::Kind::Int32: set32( -get32() ); break;
    case Type::Kind::Int64: set64( -get64() ); break;
    case Type::Kind::UInt8:
    case Type::Kind::UInt16:
    case Type::Kind::UInt32:
    case Type::Kind::UInt64: return raiseSignedProblem();
    case Type::Kind::Pointer: return raisePointerProblem();
    }
    return *this;
}

Register Register::operator+( ) {
    if ( type().isPointer() )
        return raisePointerProblem();

    promote();
    return *this;
}

Register Register::operator!( ) {
    zero() ? setu32( 1 ) : setu32( 0 );
    return *this;
}

Register Register::operator*=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::Int32:  set32( get32() * r.get32() );    break;
    case Type::Kind::UInt32: setu32( getu32() * r.getu32() ); break;
    case Type::Kind::Int64:  set64( get64() * r.get64() );    break;
    case Type::Kind::UInt64: setu64( getu64() * r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator/=( Register r ) {
    if ( r.zero() )
        throw exception::InternalError( "division by zero" );
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::Int32:  set32( get32() / r.get32() );    break;
    case Type::Kind::UInt32: setu32( getu32() / r.getu32() ); break;
    case Type::Kind::Int64:  set64( get64() / r.get64() );    break;
    case Type::Kind::UInt64: setu64( getu64() / r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator%=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::Int32:  set32( get32() % r.get32() );    break;
    case Type::Kind::UInt32: setu32( getu32() % r.getu32() ); break;
    case Type::Kind::Int64:  set64( get64() % r.get64() );    break;
    case Type::Kind::UInt64: setu64( getu64() % r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator+=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() )
            return raisePointerProblem();
        Register toAdd, count, itemSize;
        Type ptr;
        if ( type().isPointer() ) {
            toAdd = Register( getPtrUNumeric() );
            itemSize = Register( type().length() );
            count = r;
            ptr = type();
        }
        if ( r.type().isPointer() ) {
            toAdd = Register( r.getPtrUNumeric() );
            itemSize = Register( r.type().length() );
            count = *this;
            ptr = r.type();
        }

        itemSize *= count;
        toAdd += itemSize;
        *this = toAdd;
        type( ptr );
        return *this;
    }

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::Int32:  set32( get32() + r.get32() );    break;
    case Type::Kind::UInt32: setu32( getu32() + r.getu32() ); break;
    case Type::Kind::Int64:  set64( get64() + r.get64() );    break;
    case Type::Kind::UInt64: setu64( getu64() + r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator-=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            Register self( getPtrNumeric() );
            self -= r.getPtrNumeric();
            *this = self;
            return *this;
        }
        Register toSub, count, itemSize;
        Type ptr;
        if ( type().isPointer() ) {
            toSub = Register( getPtrNumeric() );
            itemSize = Register( type().length() );
            count = r;
            ptr = type();
        }
        if ( r.type().isPointer() )
            return raisePointerProblem();

        itemSize *= count;
        toSub -= itemSize;
        *this = toSub;
        type( ptr );
        return *this;
    }

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::Int32:  set32( get32() - r.get32() );    break;
    case Type::Kind::UInt32: setu32( getu32() - r.getu32() ); break;
    case Type::Kind::Int64:  set64( get64() - r.get64() );    break;
    case Type::Kind::UInt64: setu64( getu64() - r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator<<=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    if ( type().isSigned() )
        return raiseSignedProblem();

    promote();
    r.promote();

    int shift = 0;
    switch ( r.type().kind() ) {
    case Type::Kind::Int32:  shift = r.get32();  break;
    case Type::Kind::UInt32: shift = r.getu32(); break;
    case Type::Kind::Int64:  shift = int( r.get64() );  break;
    case Type::Kind::UInt64: shift = int( r.getu64() ); break;
    default:;
    }

    switch ( type().kind() ) {
    case Type::Kind::UInt32: setu32( getu32() << shift ); break;
    case Type::Kind::UInt64: setu64( getu64() << shift ); break;
    default:;
    }
    return *this;
}

Register Register::operator>>=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    if ( type().isSigned() )
        return raiseSignedProblem();

    promote();
    r.promote();

    int shift = 0;
    switch ( r.type().kind() ) {
    case Type::Kind::Int32:  shift = r.get32();  break;
    case Type::Kind::UInt32: shift = r.getu32(); break;
    case Type::Kind::Int64:  shift = int( r.get64() );  break;
    case Type::Kind::UInt64: shift = int( r.getu64() ); break;
    default:;
    }

    switch ( type().kind() ) {
    case Type::Kind::UInt32: setu32( getu32() >> shift ); break;
    case Type::Kind::UInt64: setu64( getu64() >> shift ); break;
    default:;
    }
    return *this;
}

Register Register::operator<( Register r ) {

    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() < r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() < r.get32();   break;
    case Type::Kind::UInt32: result = getu32() < r.getu32(); break;
    case Type::Kind::Int64:  result = get64() < r.get64();   break;
    case Type::Kind::UInt64: result = getu64() < r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator<=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() <= r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() <= r.get32();   break;
    case Type::Kind::UInt32: result = getu32() <= r.getu32(); break;
    case Type::Kind::Int64:  result = get64() <= r.get64();   break;
    case Type::Kind::UInt64: result = getu64() <= r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator>( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() > r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() > r.get32();   break;
    case Type::Kind::UInt32: result = getu32() > r.getu32(); break;
    case Type::Kind::Int64:  result = get64() > r.get64();   break;
    case Type::Kind::UInt64: result = getu64() > r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator>=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() >= r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() >= r.get32();   break;
    case Type::Kind::UInt32: result = getu32() >= r.getu32(); break;
    case Type::Kind::Int64:  result = get64() >= r.get64();   break;
    case Type::Kind::UInt64: result = getu64() >= r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator==( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() == r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() == r.get32();   break;
    case Type::Kind::UInt32: result = getu32() == r.getu32(); break;
    case Type::Kind::Int64:  result = get64() == r.get64();   break;
    case Type::Kind::UInt64: result = getu64() == r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator!=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() ) {
        if ( type().isPointer() && r.type().isPointer() ) {
            getPtrUNumeric() != r.getPtrUNumeric() ?
                *this = Register( 1 ) :
                *this = Register( 0 );
            return *this;
        }
        return raisePointerProblem();
    }

    arithmeticsConversion( r );

    bool result = false;

    switch ( type().kind() ) {
    case Type::Kind::Int32:  result = get32() != r.get32();   break;
    case Type::Kind::UInt32: result = getu32() != r.getu32(); break;
    case Type::Kind::Int64:  result = get64() != r.get64();   break;
    case Type::Kind::UInt64: result = getu64() != r.getu64(); break;
    default:;
    }
    result ?
        *this = Register( 1 ) :
        *this = Register( 0 );
    return *this;
}

Register Register::operator&=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    if ( type().isSigned() || r.type().isSigned() )
        return raiseSignedProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::UInt32: setu32( getu32() & r.getu32() ); break;
    case Type::Kind::UInt64: setu64( getu64() & r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator^=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    if ( type().isSigned() || r.type().isSigned() )
        return raiseSignedProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::UInt32: setu32( getu32() ^ r.getu32() ); break;
    case Type::Kind::UInt64: setu64( getu64() ^ r.getu64() ); break;
    default:;
    }
    return *this;
}

Register Register::operator|=( Register r ) {
    if ( type().isPointer() || r.type().isPointer() )
        return raisePointerProblem();

    if ( type().isSigned() || r.type().isSigned() )
        return raiseSignedProblem();

    arithmeticsConversion( r );

    switch ( type().kind() ) {
    case Type::Kind::UInt32: setu32( getu32() | r.getu32() ); break;
    case Type::Kind::UInt64: setu64( getu64() | r.getu64() ); break;
    default:;
    }
    return *this;
}


} // namespace common
} // namespace compiler
