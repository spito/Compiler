#pragma once

#include "Tree.h"

#include <vector>

namespace compiler {
namespace ast {

template< typename Frame >
struct Stack {
    using FrameHandle = std::unique_ptr < Frame >;

    bool emptyFrames() const {
        return _stack.empty();
    }

    template< typename... Args >
    common::Defer pushFrame( Args &&...args ) {
        _stack.emplace_back( new Frame( std::forward< Args >( args )... ) );
        return common::Defer( [this] { popFrame(); } );
    }

    Frame *topFrame() const {
        if ( _stack.empty() )
            return nullptr;
        return _stack.back().get();
    }

    Frame *topFrame( bool( Frame::*condition )( void ) const ) const {
        for ( const auto &f : common::revert( _stack ) ) {
            if ( f->*condition() )
                return f.get();
        }
        return baseFrame();
    }

    template< typename Condition >
    Frame *topFrame( Condition condition ) const {
        for ( const auto &f : common::revert( _stack ) ) {
            if ( condition( f.get() ) )
                return f.get();
        }
        return baseFrame();
    }

    Frame *baseFrame() const {
        return _stack.empty() ? nullptr : _stack.front().get();
    }

    template< typename Yield >
    void allFrames( Yield yield ) {
        for ( const auto &f : _stack ) {
            if ( !yield( f.get() ) )
                break;
        }
    }

private:
    std::vector< FrameHandle > _stack;
    void popFrame() {
        _stack.pop_back();
    }
};

template<>
struct Stack< void > {
};


template< typename Frame >
struct Traversal : Stack< Frame > {
protected:
    using Base = Traversal < Frame >;
    using Tree = ast::Tree;

    Traversal() :
        _tree( nullptr )
    {}

    Traversal( const Tree &tree ) :
        _tree( &tree )
    {}

    const Tree &tree() const {
        return *_tree;
    }
private:
    const Tree *_tree;

};

} // namespace ast
} // namespace compiler
