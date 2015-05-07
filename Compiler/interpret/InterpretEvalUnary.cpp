#include "Interpret.h"

namespace compiler {
namespace interpret {

void Interpret::eval( const ast::UnaryOperator *e ) {
    eval( e->expression() );

    common::Register r1 = _info->load();
    common::Register r2;
    bool sign = false;

    switch ( e->op() ) {
    case common::Operator::LogicalNot:
        _info->remember( !r1 );
        break;
    case common::Operator::AddressOf:
        if ( !_info->lValue() )
            throw exception::InternalError( "not l-value" );

        if ( _info->type()->kind() == ast::type::Kind::Elementary )
            sign = _info->type()->as< ast::type::Elementary >()->isSigned();
        _info->remember( common::Register( _info->variable().address(), _info->type()->size(), sign ) );

        break;
    case common::Operator::Dereference:
        if ( !checkRange( r1.getPtr() ) )
            throw exception::InternalError( "segfault" );
        _info->variable() = Variable( r1.getPtr(), _ast.typeStorage().addType< ast::type::Pointer >( _info->type() ) );
        break;
    case common::Operator::PrefixDecrement:
        _info->store( --r1 );
        _info->remember( r1 );
        break;
    case common::Operator::PrefixIncrement:
        _info->store( ++r1 );
        _info->remember( r1 );
        break;
    case common::Operator::SuffixDecrement:
        r2 = r1;
        _info->store( --r1 );
        _info->remember( r2 );
        break;
    case common::Operator::SuffixIncrement:
        r2 = r1;
        _info->store( ++r1 );
        _info->remember( r2 );
        break;
    case common::Operator::BitwiseNot:
        _info->remember( ~r1 );
        break;
    case common::Operator::UnaryMinus:
        _info->remember( -r1 );
        break;
    case common::Operator::UnaryPlus:
        _info->remember( +r1 );
        break;
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

} // namespace interpret
} // namespace compiler
