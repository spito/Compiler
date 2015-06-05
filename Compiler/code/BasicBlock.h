#pragma once

#include "Instruction.h"

#include <vector>
#include <set>
#include <string>

namespace compiler {
namespace code {

struct BasicBlock {

    BasicBlock( int id ) :
        _id( id )
    {}

    void makeAlive( std::string name ) {
        _name.swap( name );
    }

    int id() const {
        return _id;
    }

    const std::string &name() const {
        return _name;
    }

    void allocateRegister( std::vector< Register > registers ) {
        std::vector< Instruction > working;

        for ( Register &r : registers ) {
            working.push_back( Instruction( InstructionName::Alloc, { std::move( r ) } ) );
        }

        _instructions.insert( _instructions.begin(), working.begin(), working.end() );
    }

    void addInstruction( Instruction i ) {
        if ( open() )
            _instructions.push_back( std::move( i ) );
    }

    void addPredecessor( int blockId ) {
        _predecessors.insert( blockId );
    }

    const std::set< int > &predecessors() const {
        return _predecessors;
    }

    const std::vector< Instruction > &instructions() const {
        return _instructions;
    }

private:

    bool open() {
        if ( _instructions.empty() )
            return true;

        switch ( _instructions.back().name() ) {
        case InstructionName::Jump:
        case InstructionName::Return:
        case InstructionName::Branch:
            return false;
        default:
            return true;
        }
    }

    std::string _name;
    int _id;
    std::set< int > _predecessors;
    std::vector< Instruction > _instructions;
};

} // namespace code
} // namespace compiler
