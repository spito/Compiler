#include "Intermediate.h"

namespace compiler {
namespace generator {

static bool nextItem( std::vector< int > &vector, const std::vector< int > &domain ) {

    for ( int i = vector.size() - 1; i >= 0; --i ) {

        ++vector[ i ];

        if ( vector[ i ] < domain[ i ] )
            return true;

        if ( i == 0 )
            return false;

        vector[ i ] = 0;
    }
    return true;
}

auto Intermediate::eval( const ast::ArrayInitializer *a ) -> Operand {

    Operand variable( _namedRegisters[ *_nameMapping.find( a->variable()->name() ) ] );
    code::Type type( variable.type() );
    type.removeIndirection();
    type.removeDimension();
    type.addIndirection();

    std::vector< int > vector( a->dimensions().size(), 0 );
    int i = 0;
    do {
        Operand partial( newRegister( type ) );

        std::vector< Operand > operands{ 
            partial,
            variable,
            Operand( 0, code::Type( 31 ) ) };

        for ( int i : vector )
            operands.push_back( Operand( i, code::Type( 32 ) ) );

        addInstruction( code::InstructionName::IndexAt, std::move( operands ) );
        addInstruction( code::InstructionName::Store, {
            eval( a->values()[ i ].get() ),
            partial
        } );

        ++i;
    } while ( nextItem( vector, a->dimensions() ) );

    return code::Operand::Void();
}

} // namespace generator
} // namespace compiler
