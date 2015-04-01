#include "Position.h"
#include "File.h"

#include <list>
#include <string>
#include <algorithm>

namespace compiler {
namespace context {

static const common::Position *_position = nullptr;

static std::list< std::string > _files;
static std::string *_currentFile = nullptr;
static const std::string _emptyString;

const std::string &emptyString() {
    return _emptyString;
}

common::Position position() {
    return _position ?
        *_position :
        common::Position();
}

void position( const common::Position *position ) {
    _position = position;
}

const std::string &file() {
    return _currentFile ?
        *_currentFile :
        _emptyString;
}
void file( std::string file ) {
    auto i = std::find( _files.begin(), _files.end(), file );
    if ( i == _files.end() ) {
        _files.push_back( std::move( file ) );
        _currentFile = &_files.back();
    }
    else
        _currentFile = &*i;
}


} // namespace context
} // namespace compiler
