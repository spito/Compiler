#pragma once

#include <string>
#include <functional>
#include <memory>

namespace compiler {
    namespace common {

        struct Symbol {

            Symbol( std::string name ) :
                _name( std::move( name ) )
            {}

            virtual ~Symbol() = default;

            const std::string &name() const {
                return _name;
            }

            size_t hash() const {
                return std::hash< std::string >()( _name );
            }

            bool operator==( const Symbol &another ) const {
                return _name == another._name;
            }
            bool operator!=( const Symbol &another ) const {
                return !operator==( another );
            }

        private:
            std::string _name;
        };

        struct Keyword : Symbol {
            Keyword( std::string name ) :
                Symbol( std::move( name ) )
            {}
        };

        using SymbolHandle = std::unique_ptr < Symbol >;
        using SymbolPtr = Symbol *;

    } // namespace common
} // namespace compiler
