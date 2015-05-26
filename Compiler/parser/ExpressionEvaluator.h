#pragma once

#include "../ast/AST.h"
#include "../common/Register.h"
#include "Parser.h"

namespace compiler {
namespace parser {

struct ExpressionEvaluator : ast::Traversal {

    ExpressionEvaluator( Parser &p ) :
        _parser( p ),
        _value( 0 )
    {
        _value.quiet();
    }

    bool start( const ast::Statement *s, bool quiet = false, bool typeOnly = false ) {
        _value.quiet( quiet );
        _typeOnly = typeOnly;
        _failed = false;
        eval( s );
        return valid();
    }
    
    common::Register value() {
        _value.clearMess();
        return _value;
    }

    const ast::type::Type *type() const {
        return _type;
    }

    bool valid() const {
        return !_failed && !_value.isPointerProblem() && !_value.isSignedProblem();
    }
private:
    void eval( const ast::Statement * ) override;
    void eval( const ast::Constant * );
    void eval( const ast::Variable * );
    void eval( const ast::UnaryOperator * );
    void eval( const ast::BinaryOperator * );
    void eval( const ast::TernaryOperator * );
    void eval( const ast::Call * );

    void deduceType() {
        _type = _parser.typeStorage().addType< ast::type::Elementary >( _value.type().length(), _value.type().isSigned() );
    }

    Parser &_parser;
    common::Register _value;
    const ast::type::Type *_type;
    bool _failed;
    bool _typeOnly;
};


} // namespace parser
} // namespace compiler

