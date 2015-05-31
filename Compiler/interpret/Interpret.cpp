#include "Interpret.h"

#include <iostream>

namespace compiler {
namespace interpret {

std::map< std::string, Information( Interpret::* )( std::vector< common::Register > ) > Interpret::_intrinsicFunctions = {
        { "printf", &Interpret::intrinsicPrintf },
        { "scanf", &Interpret::intrinsicScanf },
        { "putc", &Interpret::intrinsicPutc },
};

Information Interpret::eval( const ast::Statement *s ) {

    switch ( s->kind() ) {
    case ast::Kind::Constant:
        return eval( s->as< ast::Constant >() );
    case ast::Kind::StringPlaceholder:
        return eval( s->as< ast::StringPlaceholder >() );
    case ast::Kind::ArrayInitializer:
        return eval( s->as< ast::ArrayInitializer >() );
    case ast::Kind::Variable:
        return eval( s->as< ast::Variable >() );
    case ast::Kind::UnaryOperator:
        return eval( s->as< ast::UnaryOperator >() );
    case ast::Kind::BinaryOperator:
        return eval( s->as< ast::BinaryOperator >() );
    case ast::Kind::TernaryOperator:
        return eval( s->as< ast::TernaryOperator >() );
    case ast::Kind::Call:
        return eval( s->as< ast::Call >() );
    case ast::Kind::Block:
        return eval( s->as< ast::Block >() );
    case ast::Kind::If:
        return eval( s->as< ast::If >() );
    case ast::Kind::DoWhile:
        return eval( s->as< ast::DoWhile >() );
    case ast::Kind::While:
        return eval( s->as< ast::While >() );
    case ast::Kind::For:
        return eval( s->as< ast::For >() );
    //case ast::Kind::Switch:
    //    return eval( s->as< ast::Switch >() );
    case ast::Kind::Break:
        return eval( s->as< ast::Break >() );
    case ast::Kind::Continue:
        return eval( s->as< ast::Continue >() );
    case ast::Kind::Return:
        return eval( s->as< ast::Return >() );
    }
    return Information();
}

void Interpret::start() {
    _processingGlobal = true;
    eval( &tree().global() );
    _processingGlobal = false;
    ast::Call main( common::Position(), "main", {} );
    eval( &main );
}

Variable Interpret::findSymbol( const std::string &name ) {
    if ( emptyFrames() )
        throw exception::InternalError( "empty stack" );
    Variable v;

    Frame *frame = topFrame( [&]( const Frame *f ) {
        return !f->soft() || f->find( name ).valid();
    } );

    v = frame->find( name );

    if ( !v.valid() )
        v = baseFrame()->find( name );

    if ( !v.valid() )
        throw exception::InternalError( "cannot find symbol" );
    return v;
}

bool Interpret::checkRange( const void *ptr ) {
    union Pointer {
        const char *ptr;
        uintptr_t number;
    };

    for ( const auto &p : _whitelistPointers ) {

        Pointer begin, end, examined;
        begin.ptr = p.first;
        end.ptr = p.first + p.second;
        examined.ptr = static_cast< const char * >( ptr );
        if ( begin.number <= examined.number && examined.number <= end.number )
            return true;
    }

    bool found = false;

    allFrames( [&]( const Frame *f ) {
        if ( f->containsMemoryLocation( ptr ) ) {
            found = true;
            return false;
        }
        return true;
    } );

    return found;
}

Information Interpret::eval( const ast::Variable *v ) {
    return Information( findSymbol( v->name() ) );
}

Information Interpret::eval( const ast::Constant *c ) {
    return Information( common::Register( int( c->value() ) ), c->type() );
}

Information Interpret::eval( const ast::StringPlaceholder *s ) {
    _whitelistPointers.insert( { s->value().data(), s->value().size() + 1 } );
    ast::TypeOf type = ast::TypeStorage::type( "char" ).constness().pointer();
    return Information( common::Register( const_cast<char *>( s->value().data() ), 1, true ), std::move( type ) );
}

Information Interpret::eval( const ast::ArrayInitializer *a ) {
    auto info = eval( a->variable() );

    auto value = a->values().begin();
    info.variable().flatten( [&]( Variable v ) {
        Information( v ).store( eval( value->get() ).load() );
        ++value;
    } );
    return Information();
}

Information Interpret::eval( const ast::TernaryOperator *e ) {
    auto info = eval( e->left() );

    if ( !info.load().zero() )
        return eval( e->middle() );
    else
        return eval( e->right() );
}

Information Interpret::eval( const ast::Call *e ) {
    int width = 0;

    std::vector< common::Register > values;

    for ( const auto &exp : e->parametres() ) {
        auto r = eval( exp.get() ).load();
        values.push_back( r );
        width += r.type().isPointer() ?
            sizeof( void  * ) :
            r.type().length();
    }

    auto intrinsic = _intrinsicFunctions.find( e->name() );
    if ( intrinsic != _intrinsicFunctions.end() )
        return ( this->*intrinsic->second )( values );

    const ast::Function &f = tree().findFunction( e->name() );
    if ( values.size() < f.parameters().size() )
        throw exception::InternalError( "too few arguments" );

    auto popper = pushFrame( f.parameters(), width );

    int paramIndex = 0;
    f.parameters().forOrderedVariables( [&, this]( const std::string &name, ast::MemoryHolder::Variable ) {
        Information proxy( findSymbol( name ) );
        proxy.store( values[ paramIndex ] );
        ++paramIndex;
    } );


    auto info = eval( &f.body() );

    return info;
}

Information Interpret::eval( const ast::Block *s ) {
    auto popper = pushFrame( s, !_processingGlobal );

    Information info;

    s->forDescendatns( [&]( ast::Block::Ptr p ) -> bool {
        info = eval( p );
        if ( info.skipping() )
            return false;
        return true;
    } );

    if ( !info.skipping() )
        info.clear();

    if ( _processingGlobal )
        popper.pass();

    return info;
}

Information Interpret::eval( const ast::If *s ) {
    auto popper = pushFrame( s );

    Information info;

    if ( !eval( s->condition() ).load().zero() ) {
        if ( s->ifPath() )
            info = eval( s->ifPath() );
    }
    else if ( s->elsePath() )
        info = eval( s->elsePath() );

    if ( !info.skipping() )
        info.clear();

    return info;
}

Information Interpret::eval( const ast::Break *s ) {
    Information info;
    info.breaking( true );
    return info;
}

Information Interpret::eval( const ast::Continue *s ) {
    Information info;
    info.continuing( true );
    return info;
}

Information Interpret::eval( const ast::Return *s ) {
    Information info;
    info.returning( true );

    if ( s->expression() )
        info.remember( eval( s->expression() ).load() );
    return info;
}

Information Interpret::eval( const ast::While *s ) {

    auto popper = pushFrame( s );
    topFrame()->stopAtBreak();
    topFrame()->stopAtContinue();

    Information info;

    while ( true ) {

        if ( eval( s->condition() ).load().zero() )
            break;

        info = eval( s->body() );
        if ( info.breaking() || info.returning() )
            break;
    }

    if ( !info.returning() )
        info.clear();

    return info;
}

Information Interpret::eval( const ast::DoWhile *s ) {

    topFrame()->stopAtBreak();
    topFrame()->stopAtContinue();

    Information info;

    while ( true ) {

        info = eval( s->body() );
        if ( info.breaking() || info.returning() )
            break;

        if ( eval( s->condition() ).load().zero() )
            break;
    }
    if ( !info.returning() )
        info.clear();

    topFrame()->stopAtBreak( false );
    topFrame()->stopAtContinue( false );

    return info;
}

Information Interpret::eval( const ast::For *s ) {
    auto popper = pushFrame( s );
    topFrame()->stopAtBreak();
    topFrame()->stopAtContinue();

    Information info;

    if ( s->initialization() )
        eval( s->initialization() );
    while ( true ) {

        if ( s->condition() ) {
            if ( eval( s->condition() ).load().zero() )
                break;
        }

        info = eval( s->body() );
        if ( info.breaking() || info.returning() )
            break;

        if ( s->increment() )
            eval( s->increment() );
    }
    if ( !info.returning() )
        info.clear();

    return info;
}

Information Interpret::intrinsicPrintf( std::vector< common::Register > values ) {
    if ( values.empty() )
        throw exception::InternalError( "printf: missing formating" );

    auto countBefore = std::cout.tellp();

    const char *fmt = static_cast<const char *>( values.front().getPtr() );
    bool formating = false;
    auto argument = values.begin();

    for ( ;; ++fmt ) {
        if ( !checkRange( fmt ) )
            throw exception::InternalError( "segfault" );
        if ( !*fmt )
            break;

        if ( !formating ) {
            if ( *fmt != '%' )
                std::cout << *fmt;
            else
                formating = true;
            continue;
        }
        formating = false;
        ++argument;
        if ( argument == values.end() )
            throw exception::InternalError( "printf: not enough arguments" );

        switch ( *fmt ) {
        case 'c': std::cout << argument->get8(); break;
        case 'i':
        case 'd': std::cout << argument->get32(); break;
        case 'u': std::cout << argument->getu32(); break;
        case 's': {
            const char *s = static_cast<const char *>( argument->getPtr() );
            for ( ;; ++s ) {
                if ( !checkRange( s ) )
                    throw exception::InternalError( "segfault" );
                if ( !*s )
                    break;
                std::cout << *s;
            }
        }
            break;
        default:
            throw exception::InternalError( "printf: unsupported formating specifier" );
        }
    }

    return Information( 
        common::Register( int( std::cout.tellp() - countBefore ) ),
        ast::TypeStorage::type( "int" ) );
}

Information Interpret::intrinsicScanf( std::vector< common::Register > values ) {
    if ( values.empty() )
        throw exception::InternalError( "printf: missing formating" );

    const char *fmt = static_cast<const char *>( values.front().getPtr() );
    bool formating = false;
    auto argument = values.begin();

    for ( ;; ++fmt ) {
        if ( !checkRange( fmt ) )
            throw exception::InternalError( "segfault" );
        if ( !*fmt )
            break;

        // just skip other characters
        if ( !formating ) {
            if ( *fmt == '%' )
                formating = true;
            continue;
        }
        formating = false;
        ++argument;
        if ( argument == values.end() )
            throw exception::InternalError( "scanf: not enough arguments" );

        switch ( *fmt ) {
        case 'c': std::cin >> *static_cast< int8_t * >( argument->getPtr() ); break;
        case 'i':
        case 'd': std::cin >> *static_cast< int32_t * >( argument->getPtr() ); break;
        case 'u': std::cin >> *static_cast< uint32_t * >( argument->getPtr() ); break;
        case 's': {
            char *s = static_cast< char *>( argument->getPtr() );
            for ( ;; ++s ) {
                if ( !checkRange( s ) )
                    throw exception::InternalError( "segfault" );
                char c;
                std::cin >> c;
                if ( std::cin.eof() || std::isspace( c ) ) {
                    *s = '\0';
                    break;
                }
                *s = c;
            }
        }
        default:
            throw exception::InternalError( "scanf: unsupported formating specifier" );
        }
    }

    return Information(
        common::Register( int( values.size() ) - 1 ),
        ast::TypeStorage::type( "int" ) );
}

Information Interpret::intrinsicPutc( std::vector< common::Register > values ) {
    std::cout << values.front().get8();
    return Information(
        common::Register( 1 ),
        ast::TypeStorage::type( "int" ) );
}

} // namespace interpret
} // namespace compiler
