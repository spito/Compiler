#pragma once

#include <unordered_set>

#include "../includes/exceptions.h"
#include "Symbol.h"

namespace compiler {
    namespace common {

        struct Hasher {
            template< typename H >
            auto operator()( const H &h ) const
                -> decltype ( h.hash() )
            {
                return h.hash();
            }
        };

        template<typename S >
        struct SymbolTable {
            using Symbol = S;

            void insert( Symbol &&item ) {
                if ( !_symbols.insert( item ).second )
                    throw exception::DuplicateSymbol( item );
            }
            void assign( Symbol &&item ) {
                remove( item.name() );
                insert( std::move( item ) );
            }
            const Symbol *find( const Symbol &item ) const {
                auto i = _symbols.find( item );
                if ( i == _symbols.end() )
                    return nullptr;
                return &*i;
            }
            void remove( const Symbol &item ) {
                _symbols.erase( item );
            }
            void clear() {
                _symbols.clear();
            }

        private:
            std::unordered_set< Symbol, Hasher > _symbols;
        };


    } // namespace compiler
} // namespace compiler
