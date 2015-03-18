#pragma once

#include <unordered_set>

#include "../includes/exceptions.h"
#include "Symbol.h"

namespace compiler {
    namespace common {


        struct SymbolTable {

            void insert( SymbolHandle &&item ) {
                if ( !_symbols.insert( std::move( item ) ).second )
                    throw exception::DuplicateSymbol();
            }
            bool find( const SymbolHandle &item ) {}
            void remove( const std::string &name ) {}



        private:
            std::unordered_set< SymbolHandle > _symbols;

        };


    } // namespace compiler
} // namespace compiler

namespace std {

    template<>
    struct hash < compiler::common::SymbolHandle > {
        size_t operator()( compiler::common::SymbolHandle s ) {
            return s ? s->hash() : 0;
        }
    };

    template<>
    struct equal_to < compiler::common::SymbolHandle > {
        using P = compiler::common::SymbolHandle;
        bool operator()( const P &lhs, const P &rhs ) {
            if ( lhs == rhs )
                return true;
            if ( !lhs || !rhs )
                return false;
        }
    };

} // namespace std

