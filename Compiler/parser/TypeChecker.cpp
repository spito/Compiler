#include "TypeChecker.h"


namespace compiler {
namespace parser {

std::map< TypeChecker::SimpleTypes, ast::TypeOf > TypeChecker::_conversionTable = {
        { { ast::TypeOf( 4, true ), ast::TypeOf( 4, true ) }, ast::TypeOf( 4, true ) },
        { { ast::TypeOf( 4, true ), ast::TypeOf( 4, false ) }, ast::TypeOf( 4, false ) },
        { { ast::TypeOf( 4, true ), ast::TypeOf( 8, true ) }, ast::TypeOf( 8, true ) },
        { { ast::TypeOf( 4, true ), ast::TypeOf( 8, false ) }, ast::TypeOf( 8, false ) },

        { { ast::TypeOf( 4, false ), ast::TypeOf( 4, true ) }, ast::TypeOf( 4, false ) },
        { { ast::TypeOf( 4, false ), ast::TypeOf( 4, false ) }, ast::TypeOf( 4, false ) },
        { { ast::TypeOf( 4, false ), ast::TypeOf( 8, true ) }, ast::TypeOf( 8, true ) },
        { { ast::TypeOf( 4, false ), ast::TypeOf( 8, false ) }, ast::TypeOf( 8, false ) },

        { { ast::TypeOf( 8, true ), ast::TypeOf( 4, true ) }, ast::TypeOf( 8, true ) },
        { { ast::TypeOf( 8, true ), ast::TypeOf( 4, false ) }, ast::TypeOf( 8, true ) },
        { { ast::TypeOf( 8, true ), ast::TypeOf( 8, true ) }, ast::TypeOf( 8, true ) },
        { { ast::TypeOf( 8, true ), ast::TypeOf( 8, false ) }, ast::TypeOf( 8, false ) },

        { { ast::TypeOf( 8, false ), ast::TypeOf( 4, true ) }, ast::TypeOf( 8, false ) },
        { { ast::TypeOf( 8, false ), ast::TypeOf( 4, false ) }, ast::TypeOf( 8, false ) },
        { { ast::TypeOf( 8, false ), ast::TypeOf( 8, true ) }, ast::TypeOf( 8, false ) },
        { { ast::TypeOf( 8, false ), ast::TypeOf( 8, false ) }, ast::TypeOf( 8, false ) },
};

void TypeChecker::eval( const ast::Function *f ) {

    auto popper = pushFrame( f->parameters() );
    _returnType = f->returnType();

    bool allPathHaveReturn = eval( &f->body() );
    //if ( !allPathHaveReturn )
    //    warning

}

void TypeChecker::checkConvertibility( const ast::TypeOf &from, const ast::TypeOf &to, bool boolean ) const {
    checkNonVoid( from );
    checkNonVoid( to );
    switch ( from.kind() ) {
    case ast::TypeOf::Kind::Elementary:
        if ( to.kind() == ast::TypeOf::Kind::Elementary || to.kind() == ast::TypeOf::Kind::Pointer )
            return;
        break;
    case ast::TypeOf::Kind::Array:
        if ( to.kind() == ast::TypeOf::Kind::Pointer && *to.of() == *from.of() )
            return;
        break;
    case ast::TypeOf::Kind::Pointer:
        if ( ( boolean && to.kind() == ast::TypeOf::Kind::Elementary ) || from == to )
            return;
    }
    throw exception::InternalError( "type %from is not convertible to %to" );
}

void TypeChecker::checkElementarity( const ast::TypeOf &type ) const {
    checkNonVoid( type );
    if ( type.kind() != ast::TypeOf::Kind::Elementary )
        throw exception::InternalError( "type is not elementary" );
}

void TypeChecker::checkUnsignarity( const ast::TypeOf &type ) const {
    checkElementarity( type );
    if ( type.isSigned() )
        throw exception::InternalError( "type is signed" );
}

void TypeChecker::checkMutability( const ast::TypeOf &type ) const {
    checkNonVoid( type );
    checkNonArray( type );
    if ( type.isConst() )
        throw exception::InternalError( "type is const" );
}

void TypeChecker::checkNonVoid( const ast::TypeOf &type ) const {
    if ( type.count() == 0 )
        throw exception::InternalError( "type is void" );
}

void TypeChecker::checkNonArray( const ast::TypeOf &type ) const {
    if ( type.kind() == ast::TypeOf::Kind::Array )
        throw exception::InternalError( "type is array" );
}

auto TypeChecker::getGreater( const ast::TypeOf &lhs, const ast::TypeOf &rhs ) const -> Type {
    checkElementarity( lhs );
    checkElementarity( rhs );

    int bytes = std::max( lhs.bytes(), rhs.bytes() );
    if ( bytes < 4 )
        return _int;

    auto i = _conversionTable.find( SimpleTypes(
        lhs.bytes() < 4 ? _int : lhs,
        rhs.bytes() < 4 ? _int : rhs
        ) );
    if ( i == _conversionTable.end() )
        throw exception::InternalError( "unmatched type" );
    return i->second;
}

bool TypeChecker::eval( const ast::Statement *s ) {
    switch ( s->kind() ) {
    case ast::Kind::Block:
        return eval( s->as< ast::Block >() );
    case ast::Kind::If:
        return eval( s->as< ast::If >() );
    case ast::Kind::For:
        return eval( s->as< ast::For >() );
    case ast::Kind::While:
        return eval( s->as< ast::While >() );
    case ast::Kind::DoWhile:
        return eval( s->as< ast::DoWhile >() );
    case ast::Kind::Return:
        return eval( s->as< ast::Return >() );
    case ast::Kind::Constant:
        eval( s->as< ast::Constant >() );
        break;
    case ast::Kind::StringPlaceholder:
        eval( s->as< ast::StringPlaceholder >() );
        break;
    case ast::Kind::Variable:
        eval( s->as< ast::Variable >() );
        break;
    case ast::Kind::UnaryOperator:
        eval( s->as< ast::UnaryOperator >() );
        break;
    case ast::Kind::BinaryOperator:
        eval( s->as< ast::BinaryOperator >() );
        break;
    case ast::Kind::TernaryOperator:
        eval( s->as< ast::TernaryOperator >() );
        break;
    case ast::Kind::Call:
        eval( s->as< ast::Call >() );
        break;
    }
    return false;
}

bool TypeChecker::eval( const ast::Block *s ) {
    auto popper = pushFrame( s );

    bool hasReturn = false;
    // TODO: report dead code
    s->forDescendatns( [&, this]( const ast::Statement *s ) {
        if ( eval( s ) ) {
            hasReturn = true;
            return false;
        }
        return true;
    } );
    return hasReturn;
}

bool TypeChecker::eval( const ast::If *c ) {
    auto popper = pushFrame( c );

    checkConvertibility( eval( c->condition() ), _int, true );

    bool r = c->ifPath() && eval( c->ifPath() );
    r &= c->elsePath() && eval( c->elsePath() );

    return r;
}

bool TypeChecker::eval( const ast::While *c ) {
    auto popper = pushFrame( c );

    checkConvertibility( eval( c->condition() ), _int, true );

    return eval( c->body() );
}

bool TypeChecker::eval( const ast::DoWhile *c ) {
    checkConvertibility( eval( c->condition() ), _int, true );

    return eval( c->body() );
}

bool TypeChecker::eval( const ast::For *c ) {
    auto popper = pushFrame( c );

    if ( c->initialization() )
        eval( c->initialization() );

    if ( c->condition() )
        checkConvertibility( eval( c->condition() ), _int, true );

    if ( c->increment() )
        eval( c->increment() );


    return eval( c->body() );
}

bool TypeChecker::eval( const ast::Return *r ) {

    return true;
}

auto TypeChecker::eval( const ast::Expression *e ) -> Type {
    switch ( e->kind() ) {
    case ast::Kind::Constant:
        return eval( e->as< ast::Constant >() );
    case ast::Kind::StringPlaceholder:
        return eval( e->as< ast::StringPlaceholder >() );
    case ast::Kind::Variable:
        return eval( e->as< ast::Variable >() );
    case ast::Kind::UnaryOperator:
        return eval( e->as< ast::UnaryOperator >() );
    case ast::Kind::BinaryOperator:
        return eval( e->as< ast::BinaryOperator >() );
    case ast::Kind::TernaryOperator:
        return eval( e->as< ast::TernaryOperator >() );
    case ast::Kind::Call:
        return eval( e->as< ast::Call >() );
    default:
        return Type();
    }
}

auto TypeChecker::eval( const ast::StringPlaceholder *s ) -> Type {
    return Type( ast::TypeStorage::type( "char" ).constness().pointer() );
}

auto TypeChecker::eval( const ast::Constant *c ) -> Type {
    return c->type();
}

auto TypeChecker::eval( const ast::Variable *v ) -> Type {
    if ( emptyFrames() )
        throw exception::InternalError( "empty stack" );
    const ast::TypeOf *t;

    Frame *frame = topFrame( [&]( const Frame *f ) {
        return f->variable( v->name() );
    } );

    t = frame->variable( v->name() );

    if ( !t )
        throw exception::InternalError( "cannot find symbol" );
    return Type( *t, true );
}

auto TypeChecker::eval( const ast::UnaryOperator *e ) -> Type {
    Type type = eval( e->expression() );

    switch ( e->op() ) {
    case common::Operator::LogicalNot:
        checkConvertibility( type, _int, true );
        return _int;
    case common::Operator::AddressOf:
        if ( !type.lvalue() )
            throw exception::InternalError( "not l-value" );
        return Type( ast::ProxyType( type ).pointer() );

    case common::Operator::Dereference:
        if ( !type.of() )
            throw exception::InternalError( "not dereferencable type" );
        return Type( *type.of(), true );

    case common::Operator::PrefixDecrement:
    case common::Operator::PrefixIncrement:
    case common::Operator::SuffixDecrement:
    case common::Operator::SuffixIncrement:
        checkNonArray( type );
        checkMutability( type );
        return type;

    case common::Operator::BitwiseNot:
        checkUnsignarity( type );
        return type;

    case common::Operator::UnaryMinus:
    case common::Operator::UnaryPlus:
        checkElementarity( type );
        return type;

    default:
        throw exception::InternalError( "invalid ast" );
    }
}

auto TypeChecker::eval( const ast::BinaryOperator *e ) -> Type {

    Type left = eval( e->left() );
    Type right = eval( e->right() );

    left.lvalue( false );
    right.lvalue( false );

    switch ( e->op() ) {
    case common::Operator::ArrayAccess:
        if ( left.kind() == ast::TypeOf::Kind::Elementary )
            throw exception::InternalError( "cannot use array access operator to elementary" );
        checkConvertibility( right, _int );
        return Type( *left.of(), true );

    case common::Operator::TypeCast:
        return left;

    case common::Operator::Multiplication:
    case common::Operator::Division:
    case common::Operator::Modulo:
        return getGreater( left, right );

    case common::Operator::Addition:
        checkNonVoid( left );
        checkNonArray( left );
        checkElementarity( right );
        if ( left.kind() == ast::TypeOf::Kind::Pointer )
            checkNonVoid( *left.of() );
        return left;

    case common::Operator::Subtraction:
        checkConvertibility( left, right );
        checkConvertibility( right, left );
        if ( left.kind() == ast::TypeOf::Kind::Pointer )
            return _long;
        return left;

    case common::Operator::BitwiseLeftShift:
    case common::Operator::BitwiseRightShift:
        checkUnsignarity( left );
        checkElementarity( right );
        return left;

    case common::Operator::LessThan:
    case common::Operator::LessThenOrEqual:
    case common::Operator::GreaterThan:
    case common::Operator::GreaterThanOrEqual:
    case common::Operator::EqualTo:
    case common::Operator::NotEqualTo:
        checkConvertibility( left, right, true );
        checkConvertibility( right, left, true );
        return _int;

    case common::Operator::BitwiseAnd:
    case common::Operator::BitwiseXor:
    case common::Operator::BitwiseOr:
        checkUnsignarity( left );
        checkUnsignarity( right );
        return getGreater( left, right );

    case common::Operator::LogicalAnd:
    case common::Operator::LogicalOr:
        checkNonVoid( left );
        checkNonVoid( right );
        return _int;

    case common::Operator::Initialization:
        checkConvertibility( right, left );
        checkElementarity( left );
        left.lvalue( true );
        return left;
    case common::Operator::Assignment:
    case common::Operator::AssignmentProduct:
    case common::Operator::AssignmentQuotient:
    case common::Operator::AssignmentRemainder:
    case common::Operator::AssignmentDifference:
        checkConvertibility( right, left );
        checkElementarity( left );
        checkMutability( left );
        left.lvalue( true );
        return left;
    case common::Operator::AssignmentSum:
        checkElementarity( right );
        checkMutability( left );
        left.lvalue( true );
        return left;
    case common::Operator::AssignmentLeftShift:
    case common::Operator::AssignmentRightShift:
        checkUnsignarity( left );
        checkElementarity( right );
        checkMutability( left );
        left.lvalue( true );
        return left;
    case common::Operator::AssignmentBitwiseAnd:
    case common::Operator::AssignmentBitwiseXor:
    case common::Operator::AssignmentBitwiseOr:
        checkUnsignarity( left );
        checkUnsignarity( right );
        checkMutability( left );
        left.lvalue( true );
        return left;
    default:
        throw exception::InternalError( "invalid ast" );
    }

}

auto TypeChecker::eval( const ast::TernaryOperator *e ) -> Type {
    checkConvertibility( eval( e->left() ), ast::TypeStorage::type( "int" ), true );
    Type middle = eval( e->middle() );
    Type right = eval( e->right() );
    checkConvertibility( middle, right );
    checkConvertibility( right, middle );
    return middle;
}

auto TypeChecker::eval( const ast::Call *c ) -> Type {

    const ast::Function &f = tree().findFunction( c->name() );

    auto i = c->parametres().begin();
    f.parameters().forOrderedVariables( [&, this]( const std::string &name, const ast::MemoryHolder::Variable &v ) {
        checkConvertibility( eval( i->get() ), v.type() );
        ++i;
    } );

    return f.returnType();
}


} // namespace parser
} // namespace compiler
