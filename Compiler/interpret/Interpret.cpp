#include "Interpret.h"

#include <iostream>

namespace compiler {
namespace interpret {

std::map< std::string, void( Interpret::* )( std::vector< common::Register > ) > Interpret::_intrinsicFunctions = {
        { "printf", &Interpret::intrinsicPrintf },
        { "scanf", &Interpret::intrinsicScanf },
        { "putc", &Interpret::intrinsicPutc },
};

void Interpret::eval( const ast::Statement *s ) {

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
}

void Interpret::start() {
    _processingGlobal = true;
    eval( &_ast.global() );
    _processingGlobal = false;
    ast::Call main( common::Position(), "main", {} );
    eval( &main );
}

Variable Interpret::findSymbol( const std::string &name ) {
    if ( _frames.empty() )
        throw exception::InternalError( "empty stack" );
    Variable v;

    for ( auto frame = _frames.rbegin(); frame != _frames.rend(); ++frame ) {
        v = frame->find( name );

        if ( v.valid() || !frame->soft() )
            break;
    }

    if ( !v.valid() )
        v = _frames.front().find( name );

    if ( !v.valid() )
        throw exception::InternalError( "cannot find symbol" );
    return v;
}

void Interpret::addRegister() {
    std::unique_ptr< Information > handle( _info.get() );

    if ( _frames.empty() )
        throw exception::InternalError( "empty stack" );

    _frames.back().addRegister( handle.release() );
}

void Interpret::addRegister( bool( *selector )( FrameIterator ) ) {
    std::unique_ptr< Information > handle( _info.get() );

    if ( _frames.empty() )
        throw exception::InternalError( "empty stack" );

    for ( auto frame = _frames.rbegin(); frame != _frames.rend(); ++frame ) {
        if ( selector( frame ) ) {
            frame->addRegister( handle.release() );
            return;
        }
    }
    throw exception::InternalError( "not suitable frame" );
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

    for ( auto i = _frames.rbegin(); i != _frames.rend(); ++i ) {

        if ( i->containsMemoryLocation( ptr ) )
            return true;
    }
    return false;
}

void Interpret::eval( const ast::Variable *v ) {
    _info = new Information( findSymbol( v->name() ) );
    addRegister();
}

void Interpret::eval( const ast::Constant *c ) {
    _info = new Information( common::Register( int( c->value() ) ), c->type() );
    addRegister();
}

void Interpret::eval( const ast::StringPlaceholder *s ) {
    _whitelistPointers.insert( { s->value().data(), s->value().size() + 1 } );
    const ast::type::Type *type = _ast.typeStorage().addType< ast::type::Pointer >( _ast.typeStorage().fetchType( "const char" ) );
    _info = new Information( common::Register( const_cast< char * >( s->value().data() ), 1, true ), type );
    addRegister();
}

void Interpret::eval( const ast::ArrayInitializer *a ) {
    eval( a->variable() );

    auto value = a->values().begin();
    Information *info = _info.get();
    info->variable().flatten( [&]( Variable v ) {
        eval( value->get() );
        Information( v ).store( _info->load() );
        ++value;
    } );
}

void Interpret::eval( const ast::TernaryOperator *e ) {
    eval( e->left() );

    if ( !_info->load().zero() )
        eval( e->middle() );
    else
        eval( e->right() );
}

void Interpret::eval( const ast::Call *e ) {
    int width = 0;

    std::vector< common::Register > values;

    for ( const auto &exp : e->parametres() ) {
        eval( exp.get() );
        auto r = _info->load();
        values.push_back( r );
        width += r.type().isPointer() ?
            sizeof( void  * ) :
            r.type().length();
    }

    auto intrinsic = _intrinsicFunctions.find( e->name() );
    if ( intrinsic != _intrinsicFunctions.end() ) {
        ( this->*intrinsic->second )( values );
        return;
    }

    const ast::Function *f = _ast.findFunction( e->name() );
    if ( values.size() < f->parameters().size() )
        throw exception::InternalError( "too few arguments" );

    _frames.emplace_back( f->parameters(), width );

    int paramIndex = 0;
    f->parameters().forOrderedVariables( [&, this]( const std::string &name, ast::MemoryHolder::Variable ) {
        Information proxy( findSymbol( name ) );
        proxy.store( values[ paramIndex ] );
        ++paramIndex;
    } );

    eval( &f->body() );

    auto info = new Information;
    if ( _info )
        info->remember( _info->load() );
    _info = info;

    _frames.pop_back();

    addRegister();
}

void Interpret::eval( const ast::Block *s ) {
    if ( _processingGlobal )
        _frames.emplace_back( s, false );
    else
        _frames.emplace_back( s );

    s->forDescendatns( [&]( ast::Block::Ptr p ) -> bool {
        eval( p );
        if ( _info && _info->skipping() )
            return false;
        return true;
    } );

    if ( _info && !_info->skipping() )
        _info = nullptr;

    if ( !_processingGlobal )
        _frames.pop_back();
}

void Interpret::eval( const ast::If *s ) {
    _frames.emplace_back( s );

    eval( s->condition() );
    if ( !_info->load().zero() ) {
        if ( s->ifPath() )
            eval( s->ifPath() );
    }
    else if ( s->elsePath() )
        eval( s->elsePath() );

    if ( _info && !_info->skipping() )
        _info = nullptr;

    _frames.pop_back();
}

void Interpret::eval( const ast::Break *s ) {
    if ( !s->parentBreak() )
        throw exception::InternalError( "continue without cycle" );
    _info = new Information();
    _info->breaking( true );
    addRegister( []( FrameIterator i ) -> bool {
        return i->breakStop();
    } );
}

void Interpret::eval( const ast::Continue *s ) {
    if ( !s->parentContinue() )
        throw exception::InternalError( "continue without cycle" );
    _info = new Information();
    _info->continuing( true );
    addRegister( []( FrameIterator i ) -> bool {
        return i->continueStop();
    } );
}

void Interpret::eval( const ast::Return *s ) {
    auto info = new Information();
    info->returning( true );

    if ( s->expression() ) {
        eval( s->expression() );
        info->remember( _info->load() );
    }

    _info = info;
    addRegister( []( FrameIterator i ) -> bool {
        return !i->soft();
    } );
}

void Interpret::eval( const ast::While *s ) {
    _frames.emplace_back( s );

    _frames.back().stopAtBreak();
    _frames.back().stopAtContinue();

    while ( true ) {

        eval( s->condition() );
        if ( _info->load().zero() )
            break;

        eval( s->body() );
        if ( _info && ( _info->breaking() || _info->returning() ) )
            break;
    }

    if ( _info && !_info->returning() )
        _info = nullptr;

    _frames.pop_back();
}

void Interpret::eval( const ast::DoWhile *s ) {

    _frames.back().stopAtBreak();
    _frames.back().stopAtContinue();

    while ( true ) {

        eval( s->body() );
        if ( _info && ( _info->breaking() || _info->returning() ) )
            break;

        eval( s->condition() );
        if ( _info->load().zero() )
            break;
    }
    if ( _info && !_info->returning() )
        _info = nullptr;

    _frames.back().stopAtBreak( false );
    _frames.back().stopAtContinue( false );
}

void Interpret::eval( const ast::For *s ) {
    _frames.emplace_back( s );

    _frames.back().stopAtBreak();
    _frames.back().stopAtContinue();

    if ( s->initialization() )
        eval( s->initialization() );
    while ( true ) {

        if ( s->condition() ) {
            eval( s->condition() );
            if ( _info->load().zero() )
                break;
        }

        eval( s->body() );
        if ( _info && ( _info->breaking() || _info->returning() ) )
            break;

        if ( s->increment() )
            eval( s->increment() );
    }
    if ( _info && !_info->returning() )
        _info = nullptr;

    _frames.pop_back();
}

void Interpret::intrinsicPrintf( std::vector< common::Register > values ) {
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
}

void Interpret::intrinsicScanf( std::vector< common::Register > values ) {
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
}

void Interpret::intrinsicPutc( std::vector< common::Register > values ) {
    std::cout << values.front().get8();
}

} // namespace interpret
} // namespace compiler
