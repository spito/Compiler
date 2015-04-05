#include "Worker.h"
#include "Substituer.h"

#include <fstream>

namespace compiler {
namespace preprocessor {

using Type = common::Token::Type;
using Token = common::Token;
using Operator = common::Operator;
using Position = common::Position;

std::map< std::string, void( Worker::* )( void ) > Worker::_keywords{
        { "define", &Worker::processDefine },
        { "pragma", &Worker::processPragma },
        { "undef", &Worker::processUndef },
        { "include", &Worker::processInclude },
        { "ifdef", &Worker::processIfdef },
        { "ifndef", &Worker::processIfndef },
        { "else", &Worker::processElse },
        { "endif", &Worker::processEndif },
};

void Worker::processText() {


    bool quit = false;
    store().push( Token( Type::FileBegin ) );
    while ( !quit ) {
        Token token = _tokenizer.readToken();

        switch ( token.type() ) {
        case Type::NewLine:
            _ready = true;
            break;
        case Type::Word:
            if ( !ignore() )
                store().push( Substituer( symbols(), token, _tokenizer ).result() );
            _ready = false;
            break;
        case Type::Operator:
            if ( token.op() == Operator::Sharp ) {
                if ( _ready ) {
                    _ready = false;
                    invokeExpression();
                    break;
                }
                else
                    throw exception::InvalidToken( token );
            }
            if ( token.op() == Operator::TwoShaprs )
                throw exception::InvalidToken( token );
        case Type::Char:
        case Type::Integer:
        case Type::Real:
        case Type::String:
        case Type::StringInclude:
            if ( !ignore() )
                store().push( std::move( token ) );
            _ready = false;
            break;
        case Type::Eof:
            if ( !_stack.empty() ) {
                throw exception::InternalError( "endif is missing" );
                // throw exception::MissingEnding( "endif", _stack.top().lastUsed );
            }
            store().push( Token( Type::FileEnd ) );
            quit = true;
            continue;
        default:
            throw exception::InvalidToken( token );
        }
    }

}

void Worker::invokeExpression() {
    Position before;

    Token token = _tokenizer.readToken();

    if ( token.type() != Type::Word )
        throw exception::InvalidToken( token );

    auto call = _keywords.find( token.value() );
    if ( call == _keywords.end() )
        throw exception::InvalidCharacterConstant( token.value(), before );

    (this->*call->second)();
}

void Worker::processDefine() {
    if ( ignore() )
        return;

    Token token;
    std::string name;
    std::vector< Token > value;
    std::vector< Token > params;

    Position before = position();

    token = _tokenizer.readToken( true );
    if ( token.type() != Type::Space )
        throw exception::InvalidToken( token );

    token = _tokenizer.readToken();
    if ( token.type() != Type::Word )
        throw exception::InvalidToken( token );

    Symbol symbol( token.value() );
    if ( symbol.name() == "defined" )
        throw exception::DuplicateSymbol( symbol, token.position() );

    token = _tokenizer.readToken();

    // parse formal parameters
    if ( token.isOperator( Operator::BracketOpen ) ) {
        token = _tokenizer.readToken();
        while ( true ) {

            token = _tokenizer.readToken();

            if ( token.type() != Type::Word )
                throw exception::InvalidToken( token );
            params.push_back( token );

            token = _tokenizer.readToken();

            if ( token.isOperator( Operator::Comma ) )
                continue;
            if ( token.isOperator( Operator::BracketClose ) )
                break;
            throw exception::InvalidToken( token );
        }
    }

    while ( true ) {
        if ( token.type() == Type::NewLine || token.type() == Type::Eof ) {
            _tokenizer.giveBack( token );
            break;

        value.push_back( std::move( token ) );
        token = _tokenizer.readToken();
    }

    if ( value.size() == 1 && value.back().type() == Type::Integer )
        symbol = Symbol( name, value.back() );
    else if ( !params.empty() )
        symbol = Symbol( name, params, value );
    else if ( !value.empty() )
        symbol = Symbol( name, value );

    if ( auto fetched = symbols().find( symbol ) ) {
        if ( !symbol.identical( *fetched ) )
            throw exception::InternalError( "Redefinition of symbol", before );
    }
    symbols().insert( std::move( symbol ) );
}

void Worker::processUndef() {
    if ( ignore() )
        return;

    Token token;
    std::string name;

    token = _tokenizer.readToken();
    if ( token.type() != Type::Word )
        throw exception::InvalidToken( token );

    name = token.value();

    token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );

    symbols().remove( name );
}

void Worker::processInclude() {
    if ( ignore() )
        return;

    Token token;
    Position before = position();

    token = _tokenizer.readToken();
    if ( token.type() == Type::Word ) {
        Substituer substituer( symbols(), token, _tokenizer );
        if ( substituer.result().empty() )
            throw exception::InvalidToken( token );
        if ( substituer.result().size() != 1 ||
             substituer.result().front().type() != Type::String )
             throw exception::InvalidToken( token );
    }

    std::string name = token.value();
    token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );

    context::file( name );
    if ( !_global.seenFiles.count( &context::file() ) )
        Worker w( name, _global );
    context::file( *_name );
}

void Worker::processPragma() {
    if ( ignore() )
        return;

    std::vector< Token > chunks;

    while ( true ) {
        Token token = _tokenizer.readToken();
        if ( token.type() == Type::NewLine || token.type() == Type::Eof ) {
            break;
        }
        chunks.push_back( std::move( token ) );
    }

    if ( chunks.size() == 1 && chunks.front().value() == "once" )
        _global.seenFiles.insert( &context::file() );
}

void Worker::processIfdef() {
    Token token = _tokenizer.readToken();

    if ( token.type() != Type::Word )
        throw exception::InvalidToken( token );

    std::string symbol = token.value();

    bool inherited = ignore();

    _stack.emplace(); // = push
    ConditionFrame &frame = _stack.top();

    token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );

    if ( inherited ) {
        frame.ignore = true;
        frame.inherited = true;
    }
    else if ( symbols().find( symbol ) )
        frame.fulfilled = true;
    else
        frame.ignore = true;
}

void Worker::processIfndef() {
    Token token = _tokenizer.readToken();

    if ( token.type() != Type::Word )
        throw exception::InvalidToken( token );

    std::string symbol = token.value();

    bool inherited = ignore();

    _stack.emplace(); // = push
    ConditionFrame &frame = _stack.top();

    token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );

    if ( inherited ) {
        frame.ignore = true;
        frame.inherited = true;
    }
    else if ( !symbols().find( symbol ) )
        frame.fulfilled = true;
    else
        frame.ignore = true;
}

    if ( inherited ) {
    }
    else
}

void Worker::processElse() {
    Token token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );


    if ( _stack.empty() )
        throw exception::InternalError( "missing if/ifdef/ifndef" );

    ConditionFrame &frame = _stack.top();
    if ( frame.exhausted )
        throw exception::InternalError( "missing if/ifdef/ifndef" );

    frame.exhausted = true;

    if ( frame.inherited )
        return;

    if ( frame.fulfilled )
        frame.ignore = true;
    else
        frame.ignore = false;
}

void Worker::processEndif() {
    Token token = _tokenizer.readToken();
    if ( token.type() != Type::NewLine && token.type() != Type::Eof )
        throw exception::InvalidToken( token );
    _tokenizer.giveBack( token );

    if ( _stack.empty() )
        throw exception::InternalError( "missing if/ifdef/ifndef" );

    _stack.pop();
}


} // namespace preprocessor
} // namespace compiler


