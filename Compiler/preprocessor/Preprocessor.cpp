#include "Preprocessor.h"
#include "Worker.h"


namespace compiler {
namespace preprocessor {

using common::Token;
using Type = common::Token::Type;
using Table = common::SymbolTable < Symbol > ;

Preprocessor::Preprocessor( const std::string &name ) {

    context::file( name );

    setPredefined();
    Worker w( name, _global );
    _global.store.push( common::Token() );
}

static std::vector< Token > substituteFile( const Token &token, const std::vector< std::vector< Token > > * ) {
    return std::vector < Token > { Token( token.position().file(), Type::String, token.position() ) };
}
static std::vector< Token > substituteLine( const Token &token, const std::vector< std::vector< Token > > * ) {
    Token t( std::to_string( token.position().line() ), Type::Integer, token.position() );
    t.integer() = token.position().line();
    return std::vector< Token >{ t };
}
static std::vector< Token > substituteDefined( const Table &symbols, const Token &token, const std::vector< std::vector< Token > > *params ) {
    if ( params->size() != 1 )
        throw exception::InternalError( "only one macro name can be passed into defined operator" );

    Token t( "0", Type::Integer, token.position() );
    t.integer() = 0;
    if ( symbols.find( params->front().front().value() ) ) {
        t.value() = "1";
        t.integer() = 1;
    }
    return std::vector< Token >{ t };
}

void Preprocessor::setPredefined() {

    using namespace std::placeholders;

    _global.symbols.assign( Symbol::makeSpecial( "defined", std::bind( substituteDefined, std::ref( _global.symbols ), _1, _2 ), 1, true ) );

    _global.symbols.assign( Symbol::makeSpecial( "__FILE__", substituteFile ) );
    _global.symbols.assign( Symbol::makeSpecial( "__LINE__", substituteLine ) );

#ifdef _WIN32
    _global.symbols.assign( Symbol( "_WIN32" ) );
#endif
#ifdef __unix
    _global.symbols.assign( Symbol( "__unix" ) );
#endif

}

} // namespace preprocessor
} // namespace compiler


