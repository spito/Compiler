#pragma once

#include "InputBuffer.h"

namespace compiler {
namespace common {

// Deterministic finite automaton for processing numbers
class NumberParser {
    InputBuffer &_input;

    enum class States {
        Init,
        Minus,
        Zero,
        Digits,
        Point,
        DecimalDigits,
        E,
        EPlusMinus,
        EDigits,
        Quit
    };

    bool _isReal;
    bool _isMinus;
    bool _isE;
    bool _isExponentMinus;
    States _state;
    double _denominator;

    std::string _rawToken;
    long long _exponent;
    long long _integer;
    long double _real;
public:
    NumberParser( InputBuffer &buffer ) :
        _input( buffer ),
        _isReal( false ),
        _isMinus( false ),
        _isE( false ),
        _isExponentMinus( false ),
        _state( States::Init ),
        _denominator( 1.0 ),
        _rawToken(),
        _exponent( 0 ),
        _integer( 0 ),
        _real( 0.0 )
    {}

    NumberParser( const NumberParser & ) = delete;
    NumberParser &operator=( const NumberParser & ) = delete;

    void run();

    long long integer() const {
        return _integer;
    }
    long double real() const {
        return _real;
    };

    bool isInteger() const {
        return !_isReal;
    }
    bool isReal() const {
        return _isReal;
    }

    const std::string &token() {
        return _rawToken;
    }
private:
    States stateInit( char );
    States stateMinus( char );
    States stateZero( char );
    States stateDigits( char );
    States statePoint( char );
    States stateDecimalDigits( char );
    States stateE( char );
    States stateEplusMinus( char );
    States stateEDigits( char );

    void postProcessing();
    void prepare();
};
}
}
