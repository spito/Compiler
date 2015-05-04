#include "interpret/Interpret.h"

compiler::ast::AST factorial() {
    using namespace compiler::ast;
    using Operator = compiler::common::Operator;
    AST ast;
    // factorial
    Function factorial( ast.typeStorage().fetchType( "unsigned long" ), "factorial" );

    factorial.parameters().add( "n", ast.typeStorage().fetchType( "int" ) );

    std::vector< Expression * > args;
    args.push_back(
        ast.make< BinaryOperator >(
            Operator::Subtraction,
            ast.make< Variable >( "n" ),
            ast.make< Constant >( 1, ast.typeStorage().fetchType( "int" ) )
        )
    );

    factorial.body().add(
        ast.make< Return >(
            ast.make< TernaryOperator >(
                ast.make< BinaryOperator >(
                    Operator::LessThan,
                    ast.make< Variable >( "n" ),
                    ast.make< Constant >( 1, ast.typeStorage().fetchType( "int" ) ) ),
                ast.make< Constant >( 1, ast.typeStorage().fetchType( "int" ) ),
                ast.make< BinaryOperator >(
                    Operator::Multiplication,
                    ast.make< Variable >( "n" ),
                    ast.make< Call >( "factorial", args )
                )
            )
        )
    );
    // main
    Function m( ast.typeStorage().fetchType( "int" ), "main" );

    // int a;
    // unsigned long result;
    m.body().add( "a", ast.typeStorage().fetchType( "int" ) );
    m.body().add( "result", ast.typeStorage().fetchType( "unsigned long" ) );

    args.clear();
    args.push_back(
        ast.make< UnaryOperator >(
            Operator::AddressOf,
            ast.make< Variable >( "a" )
        ) );

    // scanf( "%d", &a );
    m.body().add( ast.make< Call >( "scanf", args ) );

    args.clear();
    args.push_back(
            ast.make< Variable >( "a" )
    );

    // result = factorial( a );
    m.body().add(
        ast.make< BinaryOperator >(
            Operator::Assignment,
            ast.make< Variable >( "result" ),
            ast.make< Call >( "factorial", args )
        )
    );

    args.clear();
    args.push_back(
        ast.make< Variable >( "result" )
    );

    m.body().add(
        ast.make< Call >( "printf", args ) );

    ast.add( std::move( m ) );
    ast.add( std::move( factorial ) );

    return ast;
}

compiler::ast::AST simpleSum() {
    using namespace compiler::ast;
    using Operator = compiler::common::Operator;
    AST ast;
    std::vector< Expression * > args;

    Function m( ast.typeStorage().fetchType( "int" ), "main" );

    m.body().add( "a", ast.typeStorage().fetchType( "int" ) );

    args.clear();
    args.push_back(
        ast.make< UnaryOperator >(
        Operator::AddressOf,
        ast.make< Variable >( "a" )
        ) );

    // scanf( "%d", &a );
    m.body().add( ast.make< Call >( "scanf", args ) );


    args.clear();
    args.push_back(
        ast.make< TernaryOperator >( 
            ast.make< BinaryOperator >(
                Operator::LessThan,
                ast.make< Variable >( "a" ),
                ast.make< Constant >( 1, ast.typeStorage().fetchType( "int" ) )
            ),
            ast.make< Variable >( "a" ),
            ast.make< Constant >( 0, ast.typeStorage().fetchType( "int" ) )
        )
    );
    // printf( "%d", a );
    m.body().add(
        ast.make< Call >( "printf", args ) );


    ast.add( std::move( m ) );
    return ast;
}

int main() {
    try {

        compiler::ast::AST ast = factorial();
        compiler::interpret::Interpret i( ast );
        i.start();
    }
    catch ( compiler::exception::Exception &e ) {
        std::cout << "Exception: " << e.what() <<
            " at " << e.position().line() << ":" <<
            e.position().column() << std::endl;
    }
    return 0;
}
