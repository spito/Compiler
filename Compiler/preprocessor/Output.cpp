#include "Output.h"

#include <fstream>
#include <stack>

namespace compiler {
namespace preprocessor {

struct Frame {

    Frame( int line, const std::string &file ) :
        _line( line ),
        _file( &file )
    {}

    size_t &line() {
        return _line;
    }
    const std::string &file() const {
        return *_file;
    }

private:
    size_t _line;
    const std::string *_file;
};

static bool splitBySpace( common::Token::Type type ) {
    return
        type == common::Token::Type::Word ||
        type == common::Token::Type::Integer ||
        type == common::Token::Type::Real ||
        type == common::Token::Type::Char;
}

void Output::save( const char *name ) {
    std::ofstream file( name );

    bool word = false;

    std::stack< Frame > stack;

    while ( _p.store().available() ) {

        const common::Token &token = _p.store().get();

        if ( token.type() == common::Token::Type::FileBegin ) {
            stack.push( Frame( 1, token.position().file() ) );
            continue;
        }
        if ( token.type() == common::Token::Type::FileEnd ) {
            stack.pop();
            continue;
        }

        if ( stack.empty() )
            throw exception::InternalError( "stack frame is empty" );


        if ( stack.top().line() < token.position().line() ) {
            size_t tokenLine = token.position().line();

            while ( stack.top().line() < tokenLine ) {
                file << '\n';
                stack.top().line()++;
            }
        }
        else if ( word && splitBySpace( token.type() ) )
            file << ' ';

        file << token;
        word = splitBySpace( token.type() );
    }


}

} // namespace preprocessor
} // namespace compiler
