#include "Expression.h"
#include "Declaration.h"
#include "ExpressionEvaluator.h"
#include "../ast/Tree.h"
#include "../includes/exceptions.h"


namespace compiler {
namespace parser {

using Operator = common::Operator;

enum class Asociativity : bool {
    LeftToRight,
    RightToLeft
};

static int precedenceRank( Operator op ) {
    switch ( op ) {
    case Operator::BracketOpen:
        return 0;
    case Operator::SuffixIncrement:
    case Operator::SuffixDecrement:
    case Operator::FunctionCall:
    case Operator::ArrayAccess:
        return 1;
    case Operator::PrefixIncrement:
    case Operator::PrefixDecrement:
    case Operator::UnaryPlus:
    case Operator::UnaryMinus:
    case Operator::LogicalNot:
    case Operator::BitwiseNot:
    case Operator::TypeCast:
    case Operator::Dereference:
    case Operator::AddressOf:
    case Operator::Sizeof:
        return 2;
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
        return 3;
    case Operator::Addition:
    case Operator::Subtraction:
        return 4;
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
        return 5;
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
        return 6;
    case Operator::EqualTo:
    case Operator::NotEqualTo:
        return 7;
    case Operator::BitwiseAnd:
        return 8;
    case Operator::BitwiseXor:
        return 9;
    case Operator::BitwiseOr:
        return 10;
    case Operator::LogicalAnd:
        return 11;
    case Operator::LogicalOr:
        return 12;
    case Operator::TernaryOperator:
        return 13;
    case Operator::Assignment:
    case Operator::AssignmentSum:
    case Operator::AssignmentDifference:
    case Operator::AssignmentProduct:
    case Operator::AssignmentQuotient:
    case Operator::AssignmentRemainder:
    case Operator::AssignmentLeftShift:
    case Operator::AssignmentRightShift:
    case Operator::AssignmentBitwiseAnd:
    case Operator::AssignmentBitwiseXor:
    case Operator::AssignmentBitwiseOr:
        return 14;
    case Operator::Comma:
        return 15;
    case Operator::None:
        return 16;
    default:
        throw exception::InternalError( "unrankable operator" );
    }
}

static Asociativity asociativity( Operator op ) {
    switch ( op ) {
    case Operator::SuffixIncrement:
    case Operator::SuffixDecrement:
    case Operator::FunctionCall:
    case Operator::ArrayAccess:
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
    case Operator::Addition:
    case Operator::Subtraction:
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
    case Operator::EqualTo:
    case Operator::NotEqualTo:
    case Operator::BitwiseAnd:
    case Operator::BitwiseXor:
    case Operator::BitwiseOr:
    case Operator::LogicalAnd:
    case Operator::LogicalOr:
    case Operator::Comma:
        return Asociativity::LeftToRight;
    case Operator::PrefixIncrement:
    case Operator::PrefixDecrement:
    case Operator::UnaryPlus:
    case Operator::UnaryMinus:
    case Operator::LogicalNot:
    case Operator::BitwiseNot:
    case Operator::TypeCast:
    case Operator::Dereference:
    case Operator::AddressOf:
    case Operator::Sizeof:
    case Operator::TernaryOperator:
    case Operator::Assignment:
    case Operator::AssignmentSum:
    case Operator::AssignmentDifference:
    case Operator::AssignmentProduct:
    case Operator::AssignmentQuotient:
    case Operator::AssignmentRemainder:
    case Operator::AssignmentLeftShift:
    case Operator::AssignmentRightShift:
    case Operator::AssignmentBitwiseAnd:
    case Operator::AssignmentBitwiseXor:
    case Operator::AssignmentBitwiseOr:
        return Asociativity::RightToLeft;
    default:
        throw exception::InternalError( "unasociable operator" );
    }
}

void Expression::solvePrefixAmbiguity() {
    if ( !_it || _it->type() != Type::Operator )
        return;

    switch ( _it->op() ) {
    case Operator::Ampersand: _it->op() = Operator::AddressOf; break;
    case Operator::Star: _it->op() = Operator::Dereference; break;
    case Operator::Plus: _it->op() = Operator::UnaryPlus; break;
    case Operator::Minus: _it->op() = Operator::UnaryMinus; break;
    case Operator::Increment: _it->op() = Operator::PrefixIncrement; break;
    case Operator::Decrement: _it->op() = Operator::PrefixDecrement; break;
    default: break;
    }
}

void Expression::solvePostfixAmbiguity() {
    if ( !_it || _it->type() != Type::Operator )
        return;

    switch ( _it->op() ) {
    case Operator::Ampersand: _it->op() = Operator::BitwiseAnd; break;
    case Operator::Star: _it->op() = Operator::Multiplication; break;
    case Operator::Plus: _it->op() = Operator::Addition; break;
    case Operator::Minus: _it->op() = Operator::Subtraction; break;
    case Operator::Increment: _it->op() = Operator::SuffixIncrement; break;
    case Operator::Decrement: _it->op() = Operator::SuffixDecrement; break;
    case Operator::QuestionMark: _it->op() = Operator::TernaryOperator; break;
    case Operator::BracketIndexOpen: _it->op() = Operator::ArrayAccess; break;
    case Operator::BracketOpen: _it->op() = Operator::FunctionCall; break;
    default: break;
    }
}

inline bool isCloser( Operator owner, Operator foreign ) {

    if ( precedenceRank( owner ) == precedenceRank( foreign ) )
        return asociativity( owner ) == Asociativity::RightToLeft;
    return precedenceRank( owner ) > precedenceRank( foreign );
}
inline bool isFarther( Operator owner, Operator foreign ) {

    if ( precedenceRank( owner ) == precedenceRank( foreign ) )
        return asociativity( owner ) == Asociativity::LeftToRight;
    return precedenceRank( owner ) < precedenceRank( foreign );
}

bool Expression::unary() {
    switch ( _it->op() ) {
    case Operator::SuffixIncrement:
    case Operator::SuffixDecrement:
    case Operator::PrefixIncrement:
    case Operator::PrefixDecrement:
    case Operator::UnaryPlus:
    case Operator::UnaryMinus:
    case Operator::LogicalNot:
    case Operator::BitwiseNot:
    case Operator::Dereference:
    case Operator::AddressOf:
    case Operator::Sizeof:
        return true;
    default:
        return false;
    }
}

bool Expression::binary() {
    switch ( _it->op() ) {
    case Operator::ArrayAccess:
    case Operator::TypeCast:
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulo:
    case Operator::Addition:
    case Operator::Subtraction:
    case Operator::BitwiseLeftShift:
    case Operator::BitwiseRightShift:
    case Operator::LessThan:
    case Operator::LessThenOrEqual:
    case Operator::GreaterThan:
    case Operator::GreaterThanOrEqual:
    case Operator::EqualTo:
    case Operator::NotEqualTo:
    case Operator::BitwiseAnd:
    case Operator::BitwiseXor:
    case Operator::BitwiseOr:
    case Operator::LogicalAnd:
    case Operator::LogicalOr:
    case Operator::Assignment:
    case Operator::AssignmentSum:
    case Operator::AssignmentDifference:
    case Operator::AssignmentProduct:
    case Operator::AssignmentQuotient:
    case Operator::AssignmentRemainder:
    case Operator::AssignmentLeftShift:
    case Operator::AssignmentRightShift:
    case Operator::AssignmentBitwiseAnd:
    case Operator::AssignmentBitwiseXor:
    case Operator::AssignmentBitwiseOr:
        return true;
    default:
        return false;
    }
}

bool Expression::ternary() {
    switch ( _it->op() ) {
    case Operator::TernaryOperator:
        return true;
    default:
        return false;
    }
}

bool Expression::nnary() {
    switch ( _it->op() ) {
    case Operator::FunctionCall:
        return true;
    default:
        return false;
    }
}

bool Expression::leaving() {
    switch ( _it->op() ) {
    case Operator::BracketClose: // )
    case Operator::BraceClose: // }
    case Operator::BracketIndexClose: // ]
    case Operator::Colon: // :
    case Operator::Semicolon: // ;
        return true;
    case Operator::Comma:
        return _leaveAtComma;
    default:
        return false;
    }
}

ast::Expression *Expression::functionCall( std::string name ) {
    std::vector< ast::Expression::EHandle > arguments;
    const common::Position &position = _it->position();
    if ( !_it->isOperator( Operator::BracketClose ) ) {
        while ( _it ) {

            _leaveAtComma = true;
            arguments.emplace_back( descend( Side::Left, Operator::None ) );

            if ( _it->isOperator( Operator::Comma ) ) {
                ++_it;
                continue;
            }
            if ( _it->isOperator( Operator::BracketClose ) )
                break;
            throw exception::InvalidToken( *_it );
        }
    }
    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    _leaveAtComma = false;
    ++_it;
    return new ast::Call( position, std::move( name ), std::move( arguments ) );
}

ast::Expression *Expression::descend( Side side, Operator owner ) {

    solvePrefixAmbiguity();

    ast::Expression::EHandle self;
    Operator op;


    switch ( _it->type() ) {
    case Type::Char:
        self.reset( new ast::Constant( _it->position(), _it->value().front(), ast::TypeStorage::type( "char" ) ) );
        ++_it;
        break;
    case Type::Integer:
        self.reset( new ast::Constant( _it->position(), _it->integer(), ast::TypeStorage::type( "int" ) ) );
        ++_it;
        break;
    case Type::Real:
        self.reset( new ast::Constant( _it->position(), int64_t( _it->real() ), ast::TypeStorage::type( "int" ) ) );
        ++_it;
        break;
    case Type::String:
        self.reset( new ast::StringPlaceholder( _it->position(), _it->value() ) );
        ++_it;
        break;
    case Type::Keyword:
        if ( _it->isKeyword( Keyword::Sizeof ) ) {
            ++_it;
            self.reset( sizeofExpression() );
        }
        else
            throw exception::InvalidToken( *_it );
        break;
    case Type::Word:
        self.reset( new ast::Variable( _it->position(), _it->value() ) );
        ++_it;
        break;
    case Type::Operator:
        if ( !unary() )
            throw exception::InvalidToken( *_it );
        op = _it->op();
        ++_it;
        if ( op == Operator::BracketOpen )
            self.reset( bracketExpression() );
        else
            self.reset( new ast::UnaryOperator( _it->position(), op, descend( Side::Right, op ) ) );

        if ( _evaluator.start( self.get(), true ) ) {
            ast::Expression::EHandle s( self.release() );
            self.reset( new ast::Constant( s->position(), _evaluator.value().get64(), _evaluator.type() ) );
        }
        break;
    default:
        throw exception::InvalidToken( *_it );
    }

    while ( _it ) {

        if ( _it->type() != Type::Operator )
            throw exception::InvalidToken( *_it );

        if ( leaving() )
            break;

        solvePostfixAmbiguity();

        const auto &token = *_it;
        if ( side == Side::Right && isFarther( owner, token.op() ) )
            break;

        if ( nnary() ) {
            std::unique_ptr< ast::Variable > s( self.release()->as< ast::Variable >() );
            if ( !s )
                throw exception::InvalidToken( *_it );
            ++_it;
            self.reset( functionCall( s->name() ) );
            continue;
        }
        if ( self->kind() == ast::Kind::Variable ) {
            auto v = self->as< ast::Variable >();

            if ( !_parser.getVariable( v->name() ) )
                throw exception::InternalError( "undeclared variable" );
        }

        if ( ternary() ) {
            ++_it;
            ast::Expression::EHandle middle( descend( Side::Left, Operator::None ) );
            if ( !_it->isOperator( Operator::Colon ) )
                throw exception::InvalidToken( *_it );
            ++_it;
            ast::Expression::EHandle right( descend( Side::Right, Operator::TernaryOperator ) );
            auto s = self.release();
            self.reset( new ast::TernaryOperator( token.position(), s, middle.release(), right.release() ) );
        }
        else if ( binary() ) {
            ++_it;
            Operator parent = token.isOperator( Operator::ArrayAccess ) ? Operator::None : token.op();
            ast::Expression::EHandle right( descend( Side::Right, parent ) );
            auto s = self.release();
            self.reset( new ast::BinaryOperator( token.position(), token.op(), s, right.release() ) );
            if ( token.isOperator( Operator::ArrayAccess ) ) {
                if ( !_it->isOperator( Operator::BracketIndexClose ) )
                    throw exception::InvalidToken( *_it );
                ++_it;
            }
        }
        else if ( unary() ) {
            if ( token.op() != Operator::SuffixIncrement && token.op() != Operator::SuffixDecrement )
                throw exception::InvalidToken( *_it );
            ++_it;
            auto s = self.release();
            self.reset( new ast::UnaryOperator( token.position(), token.op(), s ) );
        }
        else
            throw exception::InvalidToken( *_it );

        if ( _evaluator.start( self.get(), true ) ) {
            ast::Expression::EHandle s( self.release() );
            self.reset( new ast::Constant( s->position(), _evaluator.value().get64(), _evaluator.type() ) );
        }
    }
    return self.release();
}

ast::Expression *Expression::sizeofExpression() {
    bool brackets = _it->isOperator( Operator::BracketOpen );
    ast::Expression::EHandle result;

    if ( brackets )
        ++_it;

    Declaration d( _parser, _it );

    if ( d.decide() == Declaration::Type::TypeOnly )
        result.reset( new ast::Constant( d.begin()->position(), d.type().bytes(), ast::TypeStorage::type( "unsigned long" ) ) );
    else {
        _it = d.begin();
        ast::Expression::EHandle partial( descend( Side::Right, Operator::Sizeof ) );
        if ( _evaluator.start( partial.get(), true, true ) )
            result.reset( new ast::Constant( partial->position(), _evaluator.type().bytes(), ast::TypeStorage::type( "unsigned long" ) ) );
        else
            throw exception::InvalidToken( *d.begin() );
    }

    if ( brackets ) {
        if ( !_it->isOperator( Operator::BracketClose ) )
            throw exception::InvalidToken( *_it );
        ++_it;
    }

    return result.release();
}

ast::Expression *Expression::bracketExpression() {
    bool fa = _leaveAtComma;
    ast::Expression::EHandle result;
    _leaveAtComma = false;
    const auto &position = _it->position();

    ++_it;

    Declaration d( _parser, _it );

    if ( d.decide() == Declaration::Type::TypeOnly ) {

        switch ( d.type().kind() ) {
        case ast::TypeOf::Kind::Elementary:
        case ast::TypeOf::Kind::Pointer:
            break;
        default:
            throw exception::InternalError( "cast to not elementary or not to pointer" );
        }

        result.reset( new ast::BinaryOperator( position, Operator::TypeCast, new ast::Constant( position, 0, d.type() ), descend( Side::Right, Operator::TypeCast ) ) );
    }
    else {
        _it = d.begin();
        result.reset( descend( Side::Left, Operator::None ) );
    }

    if ( !_it->isOperator( Operator::BracketClose ) )
        throw exception::InvalidToken( *_it );
    ++_it;

    _leaveAtComma = fa;
    return result.release();
}

} // namespace parser
} // namespace compiler

