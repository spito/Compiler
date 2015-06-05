#pragma once

#include "../ast/Tree.h"

#include <unordered_map>
#include <string>

namespace compiler {
namespace generator {

struct Frame {

    Frame( int success, int fail, int next ) :
        _success( success ),
        _fail( fail ),
        _nextBlock( next )
    {}

    int success() const {
        return _success;
    }
    int fail() const {
        return _fail;
    }
    int nextBlock() const {
        return _nextBlock;
    }

private:
    int _success;
    int _fail;
    int _nextBlock;
};

} // namespace generator
} // namespace compiler
