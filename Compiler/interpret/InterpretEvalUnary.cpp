#include "Interpret.h"

namespace compiler {
namespace interpret {

Information Interpret::eval( const ast::UnaryOperator *e ) {
    Information info = eval( e->expression() );

    common::Register r1 = info.load();
    common::Register r2;
    bool sign = false;

    switch ( e->op() ) {
    case common::Operator::LogicalNot:
        info.remember( !r1 );
        return info;
    case common::Operator::AddressOf:
        if ( !info.lValue() )
            throw exception::InternalError( "not l-value" );

        sign = info.type().isSigned();
        info.remember( common::Register( info.variable().address(), info.type().bytes(), sign ) );

        return info;
    case common::Operator::Dereference:
        if ( !checkRange( r1.getPtr() ) )
            throw exception::InternalError( "segfault" );
        info.variable() = Variable( r1.getPtr(), *info.type().of() );
        return info;
    case common::Operator::PrefixDecrement:
        info.store( --r1 );
        info.remember( r1 );
        return info;
    case common::Operator::PrefixIncrement:
        info.store( ++r1 );
        info.remember( r1 );
        return info;
    case common::Operator::SuffixDecrement:
        r2 = r1;
        info.store( --r1 );
        info.remember( r2 );
        return info;
    case common::Operator::SuffixIncrement:
        r2 = r1;
        info.store( ++r1 );
        info.remember( r2 );
        return info;
    case common::Operator::BitwiseNot:
        info.remember( ~r1 );
        return info;
    case common::Operator::UnaryMinus:
        info.remember( -r1 );
        return info;
    case common::Operator::UnaryPlus:
        info.remember( +r1 );
        return info;
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

} // namespace interpret
} // namespace compiler
