#include "Declaration.h"
#include "Expression.h"
#include "Statement.h"
#include "ExpressionEvaluator.h"

namespace compiler {
namespace parser {

using Operator = common::Operator;
using common::Token;

auto Declaration::stStart() -> States {
    if ( _it->isOperator( Operator::VariadicArgument ) )
        return beVariadicPack();
    if ( _parser.isTypeKeyword( _it->value() ) )
        return toTypeword();
    return beNone();
}

auto Declaration::stTypeword() -> States {
    if ( _it->type() == Token::Type::Keyword &&  _parser.isTypeKeyword( _it->value() ) )
        return toTypeword();

    _type = ast::TypeStorage::type( common::join( _typeId, " " ) ).constness( _constness );

    if ( _it->isOperator( Operator::Star ) )
        return toStar();

    if ( _fullExpression && _it->isOperator( Operator::BracketIndexOpen ) )
        return toArray();
    if ( _fullExpression && _it->type() == Token::Type::Operator )
        return beTypeOnly();
    if ( _it->type() == Token::Type::Word )
        return toWord();
    return toError();
}

auto Declaration::stStar() -> States {
    if ( _it->isOperator( Operator::Star ) )
        return toStar();
    if ( _fullExpression && _it->isOperator( Operator::BracketIndexOpen ) )
        return toArray();
    if ( _it->type() == Token::Type::Word )
        return toWord();
    if ( _fullExpression && _it->type() == Token::Type::Operator )
        return beTypeOnly();
    return toError();
}

auto Declaration::stArray() -> States {
    if ( _it->isOperator( Operator::BracketIndexOpen ) )
        return toArray();
    if ( _it->type() == Token::Type::Operator )
        return _name.empty() ? beTypeOnly() : beVariable();
    return toError();
}

auto Declaration::stWord() -> States {
    if ( _fullExpression && _it->isOperator( Operator::BracketIndexOpen ) )
        return toArray();
    if ( _fullExpression && _it->isOperator( Operator::BracketOpen ) )
        return toOpenParametres();
    if ( _it->type() == Token::Type::Operator )
        return beVariable();
    return toError();
}

auto Declaration::stOpenParametres() -> States {
    if ( _it->isOperator( Operator::BracketClose ) )
        return toCloseParametres();
    return toDeclaration();
}

auto Declaration::stCloseParametres() -> States {
    if ( _it->isOperator( Operator::BraceOpen ) )
        return beFunction( true );
    if ( _it->isOperator( Operator::Semicolon ) )
        return beFunction( false );
    return toError();
}

auto Declaration::stDeclaration() -> States {
    if ( _it->isOperator( Operator::BracketClose ) )
        return toCloseParametres();
    if ( _it->isOperator( Operator::Comma ) )
        return toComma();
    return toError();
}
auto Declaration::stDeclarationLast() -> States {
    if ( _it->isOperator( Operator::BracketClose ) )
        return toCloseParametres();
    return toError();
}

auto Declaration::stComma() -> States {
    return toDeclaration();
}

void Declaration::stError() {
    throw exception::InvalidToken( *_it );
}

auto Declaration::toTypeword() -> States {
    if ( _it->value() == "const" ) {
        if ( !_typeId.empty() )
            return toError();
        _constness = true;
    }
    else {
        _typeId.push_back( _it->value() );
        if ( _it->value() == "void" )
            _void = true;
    }

    return States::Typeword;
}

auto Declaration::toStar() -> States {
    _type = ast::ProxyType( _type ).pointer();
    _void = false;
    return States::Star;
}

auto Declaration::toArray() -> States {
    if ( _void )
        return toError();

    ExpressionEvaluator evaluator( _parser );
    size_t range;
    ++_it;

    auto position = _it;
    ast::Expression::EHandle h( Expression( _parser, _it ).obtain() );
    evaluator.start( h.get() );
    if ( !evaluator.valid() ) {
        _it = position;
        return toError();
    }
    range = evaluator.value().get32();
    if ( range == 0 )
        return toError();

    _type = ast::ProxyType( _type ).array( range );
    if ( !_it->isOperator( Operator::BracketIndexClose ) )
        return toError();
    return States::Array;
}

auto Declaration::toWord() -> States {
    _name = _it->value();
    return States::Word;
}

auto Declaration::toOpenParametres() -> States {
    return States::OpenParameters;
}

auto Declaration::toCloseParametres() -> States {
    return States::CloseParameters;
}

auto Declaration::toDeclaration() -> States {
    Declaration descendant( _parser, _it );
    ast::Variable *v;
    bool insertion = true;
    ast::TypeOf type;

    auto decision = descendant.decide();
    _wait = true;

    switch ( decision ) {
    case Type::SingleVariable:
    case Type::TypeOnly:
        if ( descendant.type().kind() == ast::TypeOf::Kind::Array )
            type = ast::ProxyType( *descendant.type().of() ).pointer();
        else
            type = descendant.type();
    default:
        break;
    }

    switch ( decision ) {
    case Type::SingleVariable:
        v = descendant.variable();
        for ( auto &p : _parametres ) {
            if ( p.first == v->name() ) {
                insertion = false;
                break;
            }
        }
        if ( insertion )
            _parametres.emplace_back( v->name(), type );
        delete v;
        if ( !insertion )
            throw exception::InternalError( "duplicit variable name" );
    case Type::TypeOnly:
        _types.push_back( type );
        return States::NestedDeclaration;
    case Type::Void:
        if ( !_types.empty() )
            return toError();
    case Type::VariadicPack:
        _types.push_back( descendant.type() );
        return States::LastNestedDeclaration;
    default:
        return toError();
    }
}

auto Declaration::toComma() -> States {
    return States::Comma;
}

auto Declaration::toError() -> States {
    return States::Error;
}

auto Declaration::beNone() -> States {
    _declarationType = Type::None;
    return quit();
}

auto Declaration::beTypeOnly() -> States {
    if ( _void )
        return beVoid();
    _declarationType = Type::TypeOnly;
    return quit();
}

auto Declaration::beVoid() -> States {
    _type = ast::TypeStorage::type( "void" );
    _declarationType = Type::Void;
    return quit();
}

auto Declaration::beVariadicPack() -> States {
    _type = ast::TypeStorage::type( "void" );
    _declarationType = Type::VariadicPack;
    ++_it;
    return quit();
}

auto Declaration::beVariable() -> States {
    if ( _void )
        return toError();

    _variable.reset( new ast::Variable( _begin->position(), std::move( _name ) ) );
    _declarationType = Type::SingleVariable;
    return quit();
}

auto Declaration::beFunction( bool definition ) -> States {
    std::unique_ptr< ast::Function > functionHandle( new ast::Function( _type, _name, definition ) );
    _declarationType = Type::Function;
    if ( definition ) {
        for ( const auto &p : _parametres ) {
            functionHandle->parameters().add( p.first, p.second );
        }
    }
    else {
        for ( auto p : _types ) {
            functionHandle->parameters().addPrototype( p );
        }
    }


    _parser.addFunction( functionHandle.release() );
    _function = &_parser.tree().findFunction( _name );

    if ( definition ) {
        auto d = _parser.openFunction( _function );

        ++_it;
        Statement stmt( _parser, _it );
        std::unique_ptr< ast::Block > h( stmt.block() );
        _function->body().import( *h );

        if ( !_it->isOperator( Operator::BraceClose ) )
            return toError();
    }
    ++_it;
    return quit();
}

auto Declaration::quit() -> States {
    _quit = true;
    return States::Quit;
}

Declaration::Type Declaration::decide( bool fullExpression ) {
    States state = States::Start;
    _fullExpression = fullExpression;
    _declarationType = Type::None;

    while ( _it && !_quit ) {
        switch ( state ) {
        case States::Start: state = stStart(); break;
        case States::Typeword: state = stTypeword(); break;
        case States::Star: state = stStar(); break;
        case States::Array: state = stArray(); break;
        case States::Word: state = stWord(); break;
        case States::OpenParameters: state = stOpenParametres(); break;
        case States::CloseParameters: state = stCloseParametres(); break;
        case States::NestedDeclaration: state = stDeclaration(); break;
        case States::LastNestedDeclaration: state = stDeclarationLast(); break;
        case States::Comma: state = stComma(); break;
        case States::Quit: break;
        default:
        case States::Error: stError(); break;
        }

        if ( !_quit && !_wait )
            ++_it;
        _wait = false;
    }

    return _declarationType;
}

} // namespace parser
} // namespace compiler
