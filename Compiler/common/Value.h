#pragma once

#include <cstdint>
#include <algorithm>

namespace compiler {
    namespace common {




        struct Value {

            enum class Type {
                None,
                Bool,
                Integer,
                UnsignedInteger,
                Real,
                Pointer
            };

            template< typename T >
            Value() {}

            bool is( Type type ) const {
                return type == _type;
            }

            intptr_t width() const {
                return _width;
            }


        private:

            struct _Pointer {
                intptr_t value;
                char underlyingType;

                _Pointer() :
                    value( -1 ),
                    underlyingType( 0 )
                {}

                _Pointer( intptr_t value, char underlyingType ) :
                    value( value ),
                    underlyingType( underlyingType )
                {}
            };
            struct _Bool {
                bool value;
            };
            struct _Real {
                long double value;
            };
            struct _Integer {
                intmax_t value;
            };
            struct _UnsignedInteger {
                uintmax_t value;
            };


            union _U {
                _Bool _bool;
                _Integer _integer;
                _UnsignedInteger _unsignedInteger;
                _Pointer _pointer;


                _U( const _U &other ) {
                    std::copy(
                        reinterpret_cast< const char * >( &other ),
                        reinterpret_cast< const char * >( &other + sizeof( _U ) ),
                        reinterpret_cast< char * >( this )
                        );
                }
                _U() {}
            } _value;

            Type _type;
            intptr_t _width;

        };


    } // namespace common
} // namespace compiler

