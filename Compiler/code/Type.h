#pragma once

#include <vector>

namespace compiler {
namespace code {

struct Type {

    Type( int bits, bool sign, int indirection = 0 ) :
        _bits( bits ),
        _signed( sign ),
        _indirection( indirection )
    {}

    int bits() const {
        return _bits;
    }

    bool isSigned() const {
        return _signed;
    }

    int indirection() const {
        return _indirection;
    }

    const std::vector< int > dimensions() const {
        return _dimensions;
    }

    void addDimension( int d ) {
        _dimensions.push_back( d );
    }

    void addIndirection() {
        ++_indirection;
    }
    void removeIndirection() {
        --_indirection;
    }

private:
    int _bits;
    bool _signed;
    int _indirection;
    std::vector< int > _dimensions;
};

} // namespace code
} // namespace compiler
