#include "NumberParser.h"
#include "../includes/exceptions.h"

namespace compiler {
namespace common {

void NumberParser::run() {
    prepare();

    States state = States::Init;
    Position before;
    while ( true ) {
        before = _input.position();

        char c{};

        if ( _input.eof() )
            state = States::Quit;
        else
            c = _input.readChar();

        switch ( state ) {
        case States::Init:
            state = stateInit( c );
            break;
        case States::Minus:
            state = stateMinus( c );
            break;
        case States::Zero:
            state = stateZero( c );
            break;
        case States::Digits:
            state = stateDigits( c );
            break;
        case States::Point:
            state = statePoint( c );
            break;
        case States::DecimalDigits:
            state = stateDecimalDigits( c );
            break;
        case States::E:
            state = stateE( c );
            break;
        case States::EPlusMinus:
            state = stateEplusMinus( c );
            break;
        case States::EDigits:
            state = stateEDigits( c );
            break;
        default:
            break;
        }
        if ( state == States::Quit )
            break;
    }
    _input.position( before );// return back the last character

    if ( _isE )
        postProcessing();
    if ( _isMinus ) {
        if ( isReal() )
            _real *= -1;
        if ( isInteger() )
            _integer *= -1;
    }
}

NumberParser::States NumberParser::stateInit( char c ) {
    States state;
    if ( c == '-' ) {
        _isMinus = true;
        state = States::Minus;
    }
    else if ( c == '0' ) {
        state = States::Zero;
    }
    else if ( std::isdigit( c ) ) {
        state = States::Digits;
        _integer = c - '0';
    }
    else {
        throw exception::InvalidCharacter( c, "-0123456789", _input.position() );
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::stateMinus( char c ) {
    States state;
    if ( c == '0' ) {
        state = States::Zero;
    }
    else if ( std::isdigit( c ) ) {
        state = States::Digits;
        _integer = c - '0';
    }
    else {
        throw exception::InvalidCharacter( c, "0123456789", _input.position() );
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::stateZero( char c ) {
    States state;
    if ( c == '.' ) {
        state = States::Point;
        _isReal = true;
        _real = static_cast<long double>( _integer );
    }
    else if ( c == 'e' || c == 'E' ) {
        state = States::E;
        _isE = true;
    }
    else {
        return States::Quit;
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::stateDigits( char c ){
    States state;
    if ( std::isdigit( c ) ) {
        state = States::Digits;
        _integer *= 10;
        _integer += c - '0';
    }
    else if ( c == '.' ) {
        state = States::Point;
        _isReal = true;
        _real = static_cast<long double>( _integer );
    }
    else if ( c == 'e' || c == 'E' ) {
        state = States::E;
        _isReal = true;
        _isE = true;
    }
    else {
        return States::Quit;
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::statePoint( char c ){
    if ( std::isdigit( c ) ) {
        _real += ( c - '0' ) / _denominator;
        _denominator *= 10;
    }
    else {
        throw exception::InvalidCharacter( c, "0123456789", _input.position() );
    }
    _rawToken.push_back( c );
    return States::DecimalDigits;
}

NumberParser::States NumberParser::stateDecimalDigits( char c ){
    States state;
    if ( std::isdigit( c ) ) {
        state = States::DecimalDigits;
        _real += ( c - '0' ) / _denominator;
        _denominator *= 10;
    }
    else if ( c == 'e' || c == 'E' ) {
        state = States::E;
        _isE = true;
    }
    else {
        return States::Quit;
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::stateE( char c ){
    States state;
    if ( c == '-' ) {
        state = States::EPlusMinus;
        _isExponentMinus = true;
    }
    else if ( c == '+' ) {
        state = States::EPlusMinus;
    }
    else if ( std::isdigit( c ) ) {
        state = States::EDigits;
        _exponent *= 10;
        _exponent += c - '0';
    }
    else {
        throw exception::InvalidCharacter( c, "+-0123456789", _input.position() );
    }
    _rawToken.push_back( c );
    return state;
}

NumberParser::States NumberParser::stateEplusMinus( char c ){
    if ( std::isdigit( c ) ) {
        _exponent *= 10;
        _exponent += c - '0';
    }
    else {
        throw exception::InvalidCharacter( c, "0123456789", _input.position() );
    }
    _rawToken.push_back( c );
    return States::EDigits;
}

NumberParser::States NumberParser::stateEDigits( char c ){
    if ( std::isdigit( c ) ) {
        _exponent *= 10;
        _exponent += c - '0';
    }
    else {
        return States::Quit;
    }
    _rawToken.push_back( c );
    return States::EDigits;
}

void NumberParser::postProcessing(){
    if ( _isReal ) {

        if ( _isExponentMinus ) {
            while ( _exponent ) {
                _real /= 10;
                --_exponent;
            }
        }
        else {
            while ( _exponent ) {
                _real *= 10;
                --_exponent;
            }
        }
    }
}

void NumberParser::prepare(){
    _isReal = false;
    _isMinus = false;
    _isE = false;
    _isExponentMinus = false;
    _denominator = 10.0;
    _rawToken.clear();
    _exponent = 0;
    _integer = 0;
    _real = 0.0;
}
}
}
