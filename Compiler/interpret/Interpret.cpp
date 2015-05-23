#include "Interpret.h"

#include <iostream>

namespace compiler {
namespace interpret {

std::map< std::string, void( Interpret::* )( std::vector< common::Register > ) > Interpret::_intrinsicFunctions = {
        { "printf", &Interpret::intrinsicPrintf },
        { "scanf", &Interpret::intrinsicScanf },
};

void Interpret::eval( const ast::Statement *s ) {

    switch ( s->kind() ) {
    case ast::Kind::Constant:
        return eval( s->as< ast::Constant >() );
    case ast::Kind::StringPlaceholder:
        return eval( s->as< ast::StringPlaceholder >() );
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
    _info = new Information( common::Register( c->value() ) );
    addRegister();
}

void Interpret::eval( const ast::StringPlaceholder *s ) {
    _whitelistPointers.insert( { s->value().data(), s->value().size() + 1 } );
    _info = new Information( common::Register( const_cast< char * >( s->value().data() ), 1, true ) );
    addRegister();
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
    f->parameters().forVariables( [&, this]( const std::string &name, ast::MemoryHolder::Variable ) {
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
    for ( common::Register r : values ) {

        switch ( r.type().kind() ) {
        case common::Register::Type::Kind::Int8:   std::cout << r.get8() << std::endl; break;
        case common::Register::Type::Kind::UInt8:  std::cout << r.getu8() << std::endl; break;
        case common::Register::Type::Kind::Int16:  std::cout << r.get16() << std::endl; break;
        case common::Register::Type::Kind::UInt16: std::cout << r.getu16() << std::endl; break;
        case common::Register::Type::Kind::Int32:  std::cout << r.get32() << std::endl; break;
        case common::Register::Type::Kind::UInt32: std::cout << r.getu32() << std::endl; break;
        case common::Register::Type::Kind::Int64:  std::cout << r.get64() << std::endl; break;
        case common::Register::Type::Kind::UInt64: std::cout << r.getu64() << std::endl; break;
        }

    }
}

void Interpret::intrinsicScanf( std::vector< common::Register > values ) {
    for ( common::Register r : values ) {

        if ( !r.type().isPointer() )
            throw exception::InternalError( "Not-pointer passed to scanf" );

        if ( !checkRange( r.getPtr() ) )
            throw exception::InternalError( "ptr out of range" );

        switch ( r.type().kindOfPointer() ) {
        case common::Register::Type::Kind::Int8:   std::cin >> *static_cast< int8_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::UInt8:  std::cin >> *static_cast< uint8_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::Int16:  std::cin >> *static_cast< int16_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::UInt16: std::cin >> *static_cast< uint16_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::Int32:  std::cin >> *static_cast< int32_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::UInt32: std::cin >> *static_cast< uint32_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::Int64:  std::cin >> *static_cast< int64_t * >( r.getPtr() ); break;
        case common::Register::Type::Kind::UInt64: std::cin >> *static_cast< uint64_t * >( r.getPtr() ); break;
        }
    }
}

} // namespace interpret
} // namespace compiler
