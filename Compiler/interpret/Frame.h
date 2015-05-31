#pragma once

#include "Variable.h"
#include "Information.h"

#include <functional>
#include <vector>
#include <map>
#include <list>

namespace compiler {
namespace interpret {

struct Frame {

    Frame( const ast::MemoryHolder *block, bool soft = true ) :
        _soft( soft ),
        _data( block->memoryLength() )
    {
        block->forVariables( [this]( std::string name, ast::MemoryHolder::Variable v ) {
            _variables.emplace( std::move( name ), Variable( &_data.front(), v ) );
        } );
    }

    Frame( const ast::MemoryHolder &block, int width ) :
        _soft( false ),
        _data( std::max( block.memoryLength(), width ) )
    {
        block.forVariables( [this]( std::string name, ast::MemoryHolder::Variable v ) {
            _variables.emplace( std::move( name ), Variable( &_data.front(), v ) );
        } );
    }


    Variable find( const std::string &name ) const {
        auto i = _variables.find( name );
        if ( i == _variables.end() )
            return Variable();
        return i->second;
    }

    bool soft() const {
        return _soft;
    }
    bool breakStop() const {
        return _breakStop;
    }
    bool continueStop() const {
        return _continueStop;
    }

    void stopAtBreak( bool v = true ) {
        _breakStop = v;
    }
    void stopAtContinue( bool v = true ) {
        _continueStop = v;
    }

    void addRegister( Information *info ) {
        _registers.emplace_back( info );
    }

    bool containsMemoryLocation( const void *ptr ) const {
        union Pointer {
            const void *ptr;
            uintptr_t number;
        };

        Pointer begin, end, p;
        begin.ptr = &_data.front();
        end.ptr = &_data.back();
        p.ptr = ptr;
        return begin.number <= p.number && p.number <= end.number;
    }

private:
    bool _soft;
    bool _breakStop = false;
    bool _continueStop = false;
    std::vector< char > _data;
    std::map< std::string, Variable > _variables;
    std::list< std::unique_ptr< Information > > _registers;
};
} // namespace interpret
} // namespace compiler
