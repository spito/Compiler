#pragma once

#include "../common/Symbol.h"
#include "../includes/exceptions.h"
#include "../context/Context.h"

#include <vector>

namespace compiler {
    namespace preprocessor {

        struct Symbol : common::Symbol {

            using Base = common::Symbol;

            enum class Kind {
                Nothing,
                Integer,
                String,
                Function
            };

            Symbol( std::string name ) :
                Base( std::move( name ) ),
                _kind( Kind::Nothing )
            {}

            Symbol( std::string name, int value ) :
                Base( std::move( name ) ),
                _kind( Kind::Integer ),
                _integer( value )
            {
                _value.push_back( std::to_string( value ) );
            }
            Symbol( std::string name, std::vector< std::string > value ) :
                Base( std::move( name ) ),
                _kind( Kind::String ),
                _value( std::move( value ) )
            {}
            Symbol( std::string name, std::vector< std::string > parametres, std::vector< std::string > value ) :
                Base( std::move( name ) ),
                _kind( Kind::Function ),
                _value( std::move( value ) ),
                _parametres( std::move( parametres ) )
            {}

            Symbol( const Symbol & ) = default;
            Symbol( Symbol &&other ) :
                Base( std::move( other ) ),
                _kind( other._kind ),
                _integer( other._integer ),
                _value( std::move( other._value ) ),
                _parametres( std::move( other._parametres ) )
            {}
       
            Symbol &operator=( const Symbol & ) = default;
            Symbol &operator=( Symbol &&other ) {
                swap( other );
                return *this;
            }

            Kind kind() const {
                return _kind;
            }

            int integer() const {
                if ( _kind != Kind::Integer )
                    throw exception::InternalError( "Invalid usage of symbol (not an integer)" );
                return _integer;
            }
            const std::vector< std::string > &value() const {
                return _value;
            }
            const std::vector< std::string > &parametres() const {
                if ( _kind != Kind::Function )
                    throw exception::InternalError( "Invalid usage of symbol (not a function)" );
                return _parametres;
            }

            void swap( Symbol &other ) {
                using std::swap;
                swap( _kind, other._kind );
                swap( _integer, other._integer );
                swap( _value, other._value );
                swap( _parametres, other._parametres );
            }

            bool identical( const Symbol &other ) const {
                if ( kind() != other.kind() || name() != other.name() )
                    return false;

                switch ( kind() ) {
                case Symbol::Kind::Nothing:
                    return true;
                case Symbol::Kind::Integer:
                    return integer() == other.integer();
                case Symbol::Kind::String:
                    return value() == other.value();
                case Symbol::Kind::Function:
                    return value() == other.value() &&
                        parametres() == other.parametres();
                default:
                    throw exception::InternalError( "Invalid kind of symbol" );
                }
            }
            
        private:
            Kind _kind;
            int _integer;
            std::vector< std::string > _value;
            std::vector< std::string > _parametres;
        };

    } // namespace preprocessor
} // namespace compiler
