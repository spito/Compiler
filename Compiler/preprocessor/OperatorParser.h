#pragma once

#include "../common/Operator.h"
#include "InputBuffer.h"

#include <string>

namespace compiler {
namespace preprocessor {

class OperatorParser {
    InputBuffer &_buffer;
    std::string _value;
    common::Operator _kind;
public:
    OperatorParser( InputBuffer &buffer ) :
        _buffer( buffer )
    {
        stage1();
    }

    std::string &value() {
        return _value;
    }
    common::Operator op() const {
        return _kind;
    }
private:
    void stage1();
    bool stage2( char );
    bool stage3( char, char );

    void store( const char *, common::Operator );
};


} // namespace preprocessor
} // namespace compiler
