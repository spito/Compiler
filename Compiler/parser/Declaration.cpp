#include "Declaration.h"
#include "Expression.h"
#include "ExpressionEvaluator.h"

namespace compiler {
namespace parser {

Declaration::Type Declaration::decide() {
    using Operator = common::Operator;

    Type t = Type::None;
    std::vector< std::string > typeId;
    std::string name;
    ExpressionEvaluator evaluator( _parser );

    bool constness = false;
    bool incomplete = false;
    bool expectingPointer = false;
    bool array = false;

    if ( _it && _it->isOperator( Operator::VariadicArgument ) )
        return Type::VariadicPack;

    while ( _it ) {
        if ( _it->type() != common::Token::Type::Word || !_parser.isTypeKeyword( _it->value() ) )
             break;
        typeId.push_back( _it->value() );

        if ( _it->value() == "const" ) {
            if ( typeId.size() != 1 )
                throw exception::InvalidToken( *_it );
            constness = true;
        }

        else if ( _it->value() == "void" ) {
            incomplete = true;
            if ( constness )
                expectingPointer = true;
        }

        ++_it;
    }

    if ( constness && typeId.empty() )
        throw exception::InvalidToken( *_it );

    if ( typeId.empty() )
        return t;

    _type = _parser.tree().typeStorage().fetchType( common::join( typeId, " " ) );
    if ( !_type )
        throw exception::InternalError( "invalid type" );
    t = Type::TypeOnly;

    bool leave = false;
    size_t range = 0;
    while ( _it && !leave ) {

        switch ( _it->type() ) {
        case common::Token::Type::Char:
        case common::Token::Type::Integer:
        case common::Token::Type::Real:
        case common::Token::Type::String:
            throw exception::InvalidToken( *_it );
        case common::Token::Type::Operator:
            switch ( _it->op() ) {
            case Operator::BracketIndexOpen:// OpenIndex
                ++_it;
                if ( _it->type() == common::Token::Type::Integer )
                    range = size_t( _it->integer() );
                else if ( _it->type() == common::Token::Type::Char )
                    range = _it->value().front();
                else {
                    evaluator.typeOnly( false );
                    auto position = _it;
                    evaluator.eval( Expression( _parser, _it ) );
                    if ( !evaluator.valid() )
                        throw exception::InvalidToken( *position );
                    range = size_t( evaluator.value().getu64() );
                }
                _type = _parser.tree().typeStorage().addType< ast::type::Array >( _type, range );
                if ( !_it->isOperator( Operator::BracketIndexClose ) )
                    throw exception::InvalidToken( *_it );
                array = true;
                break;
            case Operator::Star: // pointer
                if ( array )
                    return t;
                _type = _parser.tree().typeStorage().addType< ast::type::Pointer >( _type );
                ++_it;
                break;
            case Operator::BracketOpen: // function
                if ( t != Type::SingleVariable )
                    throw exception::InvalidToken( *_it );
                {
                    std::vector< const ast::type::Type * > types;
                    std::vector< std::string > names;

                    t = Type::Function;
                    incomplete = false;
                    _function = new ast::Function( _type, std::move( name ) );

                    ++_it;
                    while ( _it ) {
                        if ( _it->isOperator( Operator::BracketClose ) )
                            break;
                        Declaration d( _parser, _it );
                        switch ( d.decide() ) {
                        case Type::SingleVariable:
                        {
                            auto v = d.variable();
                            names.push_back( v->name() );
                            delete v;
                        }
                        case Type::TypeOnly:
                            if ( d.typeOnly()->kind() == ast::type::Kind::Array )
                                types.push_back( _parser.typeStorage().addType< ast::type::Pointer >(
                                &d.typeOnly()->as< ast::type::Array >()->of()
                                ) );
                            else
                                types.push_back( d.typeOnly() );
                            break;
                        case Type::VariadicPack:
                            types.push_back( _parser.typeStorage().fetchType( "void" ) );
                            break;
                        default:
                            throw exception::InvalidToken( *d.end() );
                        }
                        if ( !_it->isOperator( Operator::Comma ) )
                            break;
                    }
                    if ( !_it->isOperator( Operator::BracketClose ) )
                        throw exception::InvalidToken( *_it );
                    ++_it;

                    if ( _it->isOperator( Operator::Semicolon ) ) {
                        ++_it;
                        for ( auto p : types ) {
                            _function->parameters().addPrototype( p );
                        }
                    }
                    else if ( _it->isOperator( Operator::BraceOpen ) ) {
                        _function->parameters().namePrototypes( types, names );
                        // process statement
                    }
                    else
                        throw exception::InvalidToken( *_it );
                    return t;
                }

                return t;
            default:
                leave = true;
            }
            break;
        case common::Token::Type::Word:
            if ( array )
                return t;
            if ( t == Type::SingleVariable ) {
                leave = true;
                break;
            }
            if ( _parser.isStatementKeyword( _it->value() ) )
                throw exception::InvalidToken( *_it );
            name = _it->value();
            ++_it;
            t = Type::SingleVariable;
            break;
        }
    }

    if ( incomplete )
        throw exception::InvalidToken( *begin() );

    if ( t == Type::SingleVariable )
        _variable = new ast::Variable( begin()->position(), name );
    return t;
}

} // namespace parser
} // namespace compiler
