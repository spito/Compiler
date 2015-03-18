#pragma once

#include <cstring>
#if 0
namespace compiler {
    namespace common {

        enum class To

        enum class Numeric {
            None,
            Integral,
            Real
        };

        struct TokenType {

            TokenType() :
                _name( nullptr ),
                _value( nullptr ),
                _numeric( Numeric::None )
            {}

            TokenType( const char *name, const char *value = nullptr ) :
                _name( name ),
                _value( value ),
                _numeric( Numeric::None )
            {}
            TokenType( const char *name, Numeric numeric ) :
                _name( name ),
                _value( nullptr ),
                _numeric( numeric )
            {}

            const std::string &name() const {
                return _name;
            }
            const std::string &value() const {
                return _value;
            }
            bool integral() const {
                return _numeric == Numeric::Integral;
            }
            bool real() const {
                return _numeric == Numeric::Real;
            }

            explicit operator bool() const {
                return !_name.empty();
            }
            bool symbolic() const {
                return _value.empty();
            }

            bool is( const std::string &another ) const {
                return value() == another;
            }
            bool is( const char *another ) const {
                return _value == another;
            }

            bool operator==( const TokenType &another ) {
                return _name == another._name &&
                    _value == another._value &&
                    _numeric == another._numeric;
            }

        private:
            std::string _name;
            std::string _value;
            Numeric _numeric;
        };

    } // namespace compiler
} // namespace compiler
#endif
