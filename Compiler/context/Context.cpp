#include "Context.h"

namespace compiler {
namespace context {

static common::Position *_position = nullptr;


common::Position position() {
    return _position ?
        *_position :
        common::Position();
}

void position( common::Position *position ) {
    _position = position;
}


} // namespace context
} // namespace compiler
