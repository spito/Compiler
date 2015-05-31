#pragma once

#include "Frame.h"
#include "../ast/Traversal.h"

#include <stack>

namespace compiler {
namespace parser {

struct TypeChecker : ast::Traversal< Frame > {

    TypeChecker( const Tree &tree ) :
        Base( tree ),
        _int( ast::TypeStorage::type( "int" ) ),
        _long( ast::TypeStorage::type( "long" ) )
    {
        auto popper = pushFrame( tree.global() );
        tree.forFunctions( [this]( const ast::Function *f ) {
            eval( f );
        } );
    }


private:

    struct Type : ast::TypeOf {

        Type() = default;

        Type( ast::TypeOf t, bool lvalue = false ) :
            TypeOf( std::move( t ) ),
            _lvalue( lvalue )
        {}

        void lvalue( bool v ) {
            _lvalue = v;
        }

        bool lvalue() const {
            return _lvalue;
        }

    private:
        bool _lvalue;
    };

    Type _int;
    Type _long;
    Type _returnType;



    void checkConvertibility( const ast::TypeOf &, const ast::TypeOf &, bool = false ) const;
    void checkElementarity( const ast::TypeOf & ) const;
    void checkUnsignarity( const ast::TypeOf & ) const;
    void checkMutability( const ast::TypeOf & ) const;
    void checkNonVoid( const ast::TypeOf & ) const;
    void checkNonArray( const ast::TypeOf & ) const;
    Type getGreater( const ast::TypeOf &, const ast::TypeOf & ) const;

    void eval( const ast::Function * );
    bool eval( const ast::Statement * );
    bool eval( const ast::Block * );
    bool eval( const ast::If * );
    bool eval( const ast::While * );
    bool eval( const ast::For * );
    bool eval( const ast::DoWhile * );
    bool eval( const ast::Return * );
    Type eval( const ast::Expression * );
    Type eval( const ast::StringPlaceholder * );
    Type eval( const ast::Constant * );
    Type eval( const ast::Variable * );
    Type eval( const ast::UnaryOperator * );
    Type eval( const ast::BinaryOperator * );
    Type eval( const ast::TernaryOperator * );
    Type eval( const ast::Call * );

    struct SimpleTypes {

        SimpleTypes( const ast::TypeOf &a, const ast::TypeOf &b ) :
            _data(
            ( unsigned( a.bytes() ) << 16 ) | ( unsigned( a.isSigned() ) << 1 ) |
            ( unsigned( b.bytes() ) << 8 ) | ( unsigned( b.isSigned() ) )
            )
        {}

        bool operator<( const SimpleTypes &other ) const {
            return _data < other._data;
        }

    private:
        unsigned _data;
    };

    static std::map< SimpleTypes, ast::TypeOf > _conversionTable;
};

} // namespace parser
} // namespace compiler
