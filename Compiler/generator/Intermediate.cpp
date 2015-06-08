#include "Intermediate.h"

#include <sstream>

namespace compiler {
namespace generator {

static const char registerPrefix = '%';
static const char labelPrefix = '%';
static const char namedRegisterPrefix = '%';
static const char globalPrefix = '@';

code::Register Intermediate::newRegister( code::Type type ) {
    std::ostringstream os;
    os << registerPrefix << registerIndex();
    return code::Register( os.str(), type );
}

std::string Intermediate::labelName( int *id ) {
    std::ostringstream os;
    int order = registerIndex();
    os << labelPrefix << order;
    if ( id )
        *id = order;
    return os.str();
}

std::string Intermediate::globalName() {
    std::ostringstream os;
    os << globalPrefix << globalIndex();
    return os.str();
}

std::string Intermediate::uniqueName( std::string name ) {
    std::ostringstream os;
    os << nameIndex();
    name.append( os.str() );
    return name;
}

void Intermediate::start() {
    //auto popper = pushFrame( tree().global() );
    tree().forFunctions( [this]( const ast::Function *f ) {
        eval( f );
    } );

    _code.addGlobals( _globals );

    for ( const std::string &name : _calledFunctions ) {

        const ast::Function &f = tree().findFunction( name );
        if ( f.definition() )
            continue;

        std::vector< code::Type > arguments;
       
        f.parameters().forPrototypes( [&, this]( const ast::TypeOf &t ) {
            arguments.push_back( convertType( t ) );
        } );

        _code.addDeclaration( convertType( f.returnType() ), globalPrefix + name, std::move( arguments ) );
    }
}

common::Defer Intermediate::addNamedRegisters( const ast::MemoryHolder *block ) {
    _nameMapping.addLayer();
    block->forVariables( [this]( std::string s, const ast::MemoryHolder::Variable &v ) {
        std::string name( namedRegisterPrefix + s );
        if ( _nameMapping.count( name ) )
            name = uniqueName( name );

        code::Type type( convertType( v.type() ) );
        type.addIndirection();
        _namedRegisters.push_back( code::Register( std::move( name ), type ) );
        _nameMapping.insert( std::move( s ), _namedRegisters.size() - 1 );
    } );

    return common::Defer( [this] { _nameMapping.removeLayer(); } );
}

void Intermediate::addInstruction( code::InstructionName name, std::vector< Operand > operands ) {
    _basicBlocks.at( _currentBlock ).addInstruction( code::Instruction( name, std::move( operands ) ) );
}

void Intermediate::addJump( int blockIndex ) {
    addInstruction( code::InstructionName::Jump, {
        code::Operand::label( block( blockIndex ).id() )
    } );
}

void Intermediate::addBranch( Operand operand, int successBlock, int failBlock ) {
    addInstruction( code::InstructionName::Branch, {
        code::Operand( std::move( operand ) ),
        code::Operand::label( block( successBlock ).id() ),
        code::Operand::label( block( failBlock ).id() )
    } );
}

int Intermediate::addBasicBlock() {
    int id = _basicBlocks.size();
    _basicBlocks.emplace_back( id );
    return id;
}

code::BasicBlock &Intermediate::block( int index ) {
    return _basicBlocks.at( index );
}

void Intermediate::refreshBlock( int index ) {
    _currentBlock = index;
    block( _currentBlock ).makeAlive( labelName() );
}

code::Type Intermediate::convertType( const ast::TypeOf &type ) {
    std::vector< int > dimensions;
    int indirection = 0;

    const ast::TypeOf *base = &type;
    while ( base->kind() == ast::TypeOf::Kind::Array ) {
        dimensions.push_back( base->count() );
        base = base->of();
    }

    while ( base->kind() == ast::TypeOf::Kind::Pointer ) {
        ++indirection;
        base = base->of();
    }

    if ( base->kind() != ast::TypeOf::Kind::Elementary )
        throw exception::InternalError( "invalid type" );

    code::Type result( base->bytes() * CHAR_BIT, base->isSigned(), indirection );
    for ( int d : dimensions )
        result.addDimension( d );
    return result;
}

void Intermediate::eval( const ast::Function *f ) {
    auto popper = addNamedRegisters( &f->parameters() );

    _basicBlocks.clear();
    _namedRegisters.clear();
    _registerIndex = 0;
    _nameIndex = 0;

    refreshBlock( addBasicBlock() );

    bool isReturn = eval( &f->body() );

    if ( !isReturn ) {
        addInstruction( code::InstructionName::Return, {
            code::Operand::Void()
        } );
    }

    //_basicBlocks.front().

    _code.addFunction( convertType( f->returnType() ),
                       f->name(),
                       std::move( _namedRegisters ),
                       std::move( _basicBlocks ) );
}

bool Intermediate::eval( const ast::Statement *s ) {
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
    case ast::Kind::Break:
        return eval( s->as< ast::Break >() );
    case ast::Kind::Continue:
        return eval( s->as< ast::Continue >() );

    case ast::Kind::Constant:
        eval( s->as< ast::Constant >() );
        break;
    case ast::Kind::StringPlaceholder:
        eval( s->as< ast::StringPlaceholder >() );
        break;
    case ast::Kind::ArrayInitializer:
        eval( s->as< ast::ArrayInitializer >() );
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


bool Intermediate::eval( const ast::Block *b ) {
    auto popper = addNamedRegisters( b );

    bool hasReturn = false;
    b->forDescendatns( [&, this]( const ast::Statement *s ) {
        if ( eval( s ) ) {
            hasReturn = true;
            return false;
        }
        return true;
    } );

    //if ( next >= 0 ) {
    //    addInstruction( code::Instruction( code::Instruction::Kind::Jump, {
    //        code::Operand::label( block( next ).name() )
    //    } ), next );
    //}


    return hasReturn;
}

bool Intermediate::eval( const ast::If *c ) {
    auto namePopper = addNamedRegisters( c );

    int ifPath = c->ifPath() ? addBasicBlock() : -1;
    int elsePath = c->elsePath() ? addBasicBlock() : -1;

    if ( ifPath == elsePath ) {
        eval( c->condition() );
        return false;
    }

    int next = addBasicBlock();
    ifPath = ifPath == -1 ? next : ifPath;
    elsePath = elsePath == -1 ? next : elsePath;

    auto popper = pushFrame( ifPath, elsePath, next );
    auto result = eval( c->condition() );
    popper.run();

    block( ifPath ).addPredecessor( _currentBlock );
    block( elsePath ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( ifPath );
    block( next ).addPredecessor( elsePath );

    addBranch( std::move( result ), ifPath, elsePath );

    bool hasReturn = true;
    if ( c->ifPath() ) {
        refreshBlock( ifPath );
        hasReturn = eval( c->ifPath() ) && hasReturn;
        addJump( next );
    }
    else
        hasReturn = false;

    if ( c->elsePath() ) {
        refreshBlock( elsePath );
        hasReturn = eval( c->ifPath() ) && hasReturn;
        addJump( next );
    }
    else
        hasReturn = false;

    refreshBlock( next );

    return hasReturn;
}

bool Intermediate::eval( const ast::While *c ) {
    auto namePopper = addNamedRegisters( c );

    int condition = addBasicBlock();
    int body = addBasicBlock();
    int next = addBasicBlock();

    addJump( condition );

    block( condition ).addPredecessor( _currentBlock );
    block( condition ).addPredecessor( body );
    block( body ).addPredecessor( condition );
    block( next ).addPredecessor( condition );

    auto popper = pushFrame( body, next, condition );

    refreshBlock( condition );
    auto result = eval( c->condition() );

    addBranch( std::move( result ), body, next );

    refreshBlock( body );
    bool hasReturn = eval( c->body() );
    addJump( condition );

    refreshBlock( next );

    return hasReturn;
}

bool Intermediate::eval( const ast::DoWhile *c ) {

    int condition = addBasicBlock();
    int body = addBasicBlock();
    int next = addBasicBlock();

    addJump( body );

    block( condition ).addPredecessor( body );
    block( body ).addPredecessor( _currentBlock );
    block( body ).addPredecessor( condition );
    block( next ).addPredecessor( condition );

    auto popper = pushFrame( body, next, condition );

    refreshBlock( condition );
    auto result = eval( c->condition() );

    addBranch( std::move( result ), body, next );

    refreshBlock( body );
    bool hasReturn = eval( c->body() );
    addJump( condition );

    refreshBlock( next );

    return hasReturn;
}

bool Intermediate::eval( const ast::For *c ) {

    auto namePopper = addNamedRegisters( c );

    if ( c->initialization() ) {
        int initialization = addBasicBlock();
        block( initialization ).addPredecessor( _currentBlock );
        addJump( initialization );

        _currentBlock = initialization;
        eval( c->initialization() );
    }


    int condition = -1;
    int increment = -1;
    int body = -1;

    if ( c->increment() )
        increment = addBasicBlock();

    if ( c->condition() )
        condition = addBasicBlock();

    body = addBasicBlock();
    if ( condition == -1 )
        condition = body;
    if ( increment == -1 )
        increment = condition;

    int next = addBasicBlock();

    addJump( body );

    block( condition ).addPredecessor( _currentBlock );
    block( condition ).addPredecessor( increment == condition ? body : increment );
    block( body ).addPredecessor( condition );
    block( increment ).addPredecessor( body );

    if ( condition != body )
        block( next ).addPredecessor( condition );

    auto popper = pushFrame( body, next, increment );

    if ( c->increment() ) {
        refreshBlock( increment );
        eval( c->increment() );

        addJump( condition );
    }

    if ( c->condition() ) {
        refreshBlock( condition );
        auto result = eval( c->condition() );

        addBranch( std::move( result ), body, next );
    }

    refreshBlock( body );
    bool hasReturn = eval( c->body() );
    addJump( increment );

    refreshBlock( next );

    return hasReturn;
}

bool Intermediate::eval( const ast::Break *s ) {
    int fail = topFrame()->fail();
    block( fail ).addPredecessor( _currentBlock );
    addJump( fail );

    return false;
}

bool Intermediate::eval( const ast::Continue *s ) {
    int next = topFrame()->nextBlock();
    addJump( next );

    return false;
}

bool Intermediate::eval( const ast::Return *s ) {

    auto result = code::Operand::Void();
    if ( s->expression() )
        result = eval( s->expression() );

    addInstruction( code::InstructionName::Return, {
        std::move( result )
    } );

    return true;
}

auto Intermediate::eval( const ast::Expression *e, Access access ) -> Operand {
    switch ( e->kind() ) {
    case ast::Kind::Constant:
        return eval( e->as< ast::Constant >() );
    case ast::Kind::StringPlaceholder:
        return eval( e->as< ast::StringPlaceholder >() );
    case ast::Kind::ArrayInitializer:
        return eval( e->as< ast::ArrayInitializer >() );
    case ast::Kind::Variable:
        return eval( e->as< ast::Variable >(), access );
    case ast::Kind::UnaryOperator:
        return eval( e->as< ast::UnaryOperator >(), access );
    case ast::Kind::BinaryOperator:
        return eval( e->as< ast::BinaryOperator >(), access );
    case ast::Kind::TernaryOperator:
        return eval( e->as< ast::TernaryOperator >(), access );
    case ast::Kind::Call:
        return eval( e->as< ast::Call >(), access );
    default:
        throw exception::InternalError( "invalid ast" );
    }
}

auto Intermediate::eval( const ast::Constant *c ) -> Operand {
    return Operand( c->value(), convertType( c->type() ) );
}

auto Intermediate::eval( const ast::StringPlaceholder *s ) -> Operand {
    code::Type globalType( CHAR_BIT, true );
    globalType.addDimension( s->value().size() + 1 );
    Operand op( code::Register( globalName(), globalType ) );

    std::vector< code::Operand > operands;

    operands.push_back( op );

    for ( char c : s->value() ) {
        operands.push_back( Operand( c, code::Type( CHAR_BIT, true ) ) );
    }
    operands.push_back( Operand( 0, code::Type( CHAR_BIT, true ) ) );

    _globals.emplace_back( code::Instruction( code::InstructionName::Global, std::move( operands ) ) );

    Operand result( newRegister( code::Type( CHAR_BIT, true, 1 ) ) );
    addInstruction( code::InstructionName::IndexAt, {
        result,
        op,
        Operand( 0, code::Type( CHAR_BIT * 4, true ) ),
        Operand( 0, code::Type( CHAR_BIT * 4, true ) )
    } );

    return result;
}

auto Intermediate::eval( const ast::ArrayInitializer *a ) -> Operand {

#pragma message( "warning: implement array initialization" )
    //Operand array = eval( a->variable() );

    //for ( unsigned i = 0; i < a->values().size(); ++i ) {

    //    addInstruction( code::InstructionName::IndexAt, {
    //        newRegister(  ),
    //        array,
    //        Operand(  )
    //    } );

    //}




    return code::Operand::Void();
}

auto Intermediate::eval( const ast::Variable *v, Access access ) -> Operand {

    code::Operand variable( _namedRegisters[ *_nameMapping.find( v->name() ) ] );

    if ( access == Access::Store )
        return Operand( variable );

    code::Type type( variable.type() );
    type.removeIndirection();

    code::Operand result( newRegister( type ) );
    addInstruction( code::InstructionName::Load, {
        result,
        variable
    } );

    return result;
}

auto Intermediate::eval( const ast::UnaryOperator *e, Access access ) -> Operand {

    switch ( e->op() ) {
    case common::Operator::LogicalNot:
        return opLogicalNot( eval( e->expression() ) );
    case common::Operator::AddressOf:
        return opAddressOf( eval( e->expression(), Access::Store ) );
    case common::Operator::Dereference:
        return opDereference( eval( e->expression(), access ) );
    case common::Operator::PrefixDecrement:
        return opPrefixDecrement( eval( e->expression(), Access::Store ) );
    case common::Operator::PrefixIncrement:
        return opPrefixIncrement( eval( e->expression(), Access::Store ) );
    case common::Operator::SuffixDecrement:
        return opSuffixDecrement( eval( e->expression(), Access::Store ) );
    case common::Operator::SuffixIncrement:
        return opSuffixIncrement( eval( e->expression(), Access::Store ) );
    case common::Operator::BitwiseNot:
        return opBitwiseNot( eval( e->expression() ) );
    case common::Operator::UnaryMinus:
        return opUnaryMinus( eval( e->expression() ) );
    case common::Operator::UnaryPlus:
        return eval( e->expression() );
    default:
        throw exception::InternalError( "invalid operator" );
    }
}

auto Intermediate::eval( const ast::BinaryOperator *e, Access access ) -> Operand {
    switch ( e->op() ) {
    case common::Operator::LogicalOr:
        return opLogicalOr( e, access );
    case common::Operator::LogicalAnd:
        return opLogicalAnd( e, access );
    case common::Operator::ArrayAccess:
        return opArrayAccess( eval( e->left() ),
                              eval( e->right() ),
                              access );
    case common::Operator::TypeCast:
        return opTypeCast( eval( e->left() ).type(),
                           eval( e->right() ) );
    case common::Operator::Multiplication:
        return opMultiplication( eval( e->left() ),
                                 eval( e->right() ) );
    case common::Operator::Division:
        return opDivision( eval( e->left() ),
                           eval( e->right() ) );
    case common::Operator::Modulo:
        return opModulo( eval( e->left() ),
                         eval( e->right() ) );
    case common::Operator::Addition:
        return opAddition( eval( e->left() ),
                           eval( e->right() ) );
    case common::Operator::Subtraction:
        return opSubtraction( eval( e->left() ),
                              eval( e->right() ) );
    case common::Operator::BitwiseLeftShift:
        return opBitwiseLeftShift( eval( e->left() ),
                                   eval( e->right() ) );
    case common::Operator::BitwiseRightShift:
        return opBitwiseRightShift( eval( e->left() ),
                                    eval( e->right() ) );
    case common::Operator::LessThan:
        return opLessThan( eval( e->left() ),
                           eval( e->right() ) );
    case common::Operator::LessThenOrEqual:
        return opLessThenOrEqual( eval( e->left() ),
                                  eval( e->right() ) );
    case common::Operator::GreaterThan:
        return opGreaterThan( eval( e->left() ),
                              eval( e->right() ) );
    case common::Operator::GreaterThanOrEqual:
        return opGreaterThanOrEqual( eval( e->left() ),
                                     eval( e->right() ) );
    case common::Operator::EqualTo:
        return opEqualTo( eval( e->left() ),
                          eval( e->right() ) );
    case common::Operator::NotEqualTo:
        return opNotEqualTo( eval( e->left() ),
                             eval( e->right() ) );
    case common::Operator::BitwiseAnd:
        return opBitwiseAnd( eval( e->left() ),
                             eval( e->right() ) );
    case common::Operator::BitwiseXor:
        return opBitwiseXor( eval( e->left() ),
                             eval( e->right() ) );
    case common::Operator::BitwiseOr:
        return opBitwiseOr( eval( e->left() ),
                            eval( e->right() ) );

    case common::Operator::Assignment:
    case common::Operator::Initialization:
        return opAssignment( eval( e->left(), Access::Store ),
                             eval( e->right() ) );
    case common::Operator::AssignmentProduct:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::Multiplication );
    case common::Operator::AssignmentQuotient:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::Division );
    case common::Operator::AssignmentRemainder:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::Modulo );
    case common::Operator::AssignmentSum:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::Addition );
    case common::Operator::AssignmentDifference:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::Subtraction );
    case common::Operator::AssignmentLeftShift:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::BitwiseLeftShift );
    case common::Operator::AssignmentRightShift:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::BitwiseRightShift );
    case common::Operator::AssignmentBitwiseAnd:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::BitwiseAnd );
    case common::Operator::AssignmentBitwiseXor:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::BitwiseXor );
    case common::Operator::AssignmentBitwiseOr:
        return opCompoundAssignment( eval( e->left(), Access::Store ),
                                     eval( e->right() ),
                                     common::Operator::BitwiseOr );
    default:
        throw exception::InternalError( "invalid binary operator" );
    }
}


auto Intermediate::eval( const ast::TernaryOperator *e, Access access ) -> Operand {

    int success = addBasicBlock();
    int fail = addBasicBlock();
    int next = addBasicBlock();

    block( success ).addPredecessor( _currentBlock );
    block( fail ).addPredecessor( _currentBlock );
    block( next ).addPredecessor( success );
    block( next ).addPredecessor( fail );


    Operand condition = eval( e->left() );
    addBranch( condition, success, fail );

    refreshBlock( success );
    Operand middle = eval( e->middle() );
    addJump( next );

    refreshBlock( fail );
    Operand right = eval( e->right() );
    addJump( next );

    refreshBlock( next );
    Operand result( newRegister( middle.type() ) );
    addInstruction( code::InstructionName::Merge, {
        result,
        middle,
        Operand::label( block( success ).id() ),
        right,
        Operand::label( block( fail ).id() ),
    } );

    return result;
}

auto Intermediate::eval( const ast::Call *c ) -> Operand {

    _calledFunctions.insert( c->name() );

    std::vector< Operand > operands;
    code::Type returnType( convertType( tree().findFunction( c->name() ).returnType() ) );

    operands.push_back( code::Register( globalPrefix + c->name(), returnType ) );

    for ( const auto &h : c->parametres() ) {
        operands.push_back( eval( h.get() ) );
    }

    Operand result( returnType.bits() ?
                    newRegister( returnType ) :
                    Operand::Void() );

    if ( returnType.bits() )
        operands.insert( operands.begin(), result );

    addInstruction( code::InstructionName::Call, std::move( operands ) );
    return result;
}

} // namespace generator
} // namespace compiler
