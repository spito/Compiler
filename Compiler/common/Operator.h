#pragma once

namespace compiler {
namespace common {

enum class Operator {
    None,
    // not ranked
    Ampersand, // &
    Star, // *
    Plus, // +
    Minus, // -
    Sharp, // #
    TwoShaprs, // ##
    QuestionMark, // ?
    Semicolon, // ;
    Colon, // :
    VariadicArgument, // ...


    // rank 0
    BracketOpen, // (
    BracketClose, // )
    BraceOpen, // {
    BraceClose, // }

    // rank 1, left to right
    Increment, // ++
    Decrement, // --
    BracketIndexOpen, // [
    BracketIndexClose, // ]

    // rank 2, right to left
    UnaryPlus, // +
    UnaryMinus, // -
    LogicalNot, // !
    BitwiseNot, // ~
    //TypeCast, // (int)
    Dereference, // *
    AddressOf, // &
    //Sizeof, // sizeof(int)

    // rank 3, left to right
    Multiplication, // *
    Division, // /
    Modulo, // %

    // rank 4, left to right
    Addition, // +
    Subtraction, // -

    // rank 5, left to right
    BitwiseLeftShift, // <<
    BitwiseRightShift, // >>

    // rank 6, left to right
    LessThan, // <
    LessThenOrEqual, // <=
    GreaterThan, // >
    GreaterThanOrEqual, // >=

    // rank 7, left to right
    EqualTo, // ==
    NotEqualTo, // !=

    // rank 8, left to right
    BitwiseAnd, // &

    // rank 9, left to right
    BitwiseXor, // ^

    // rank 10, left to right
    BitwiseOr, // |

    // rank 11, left to right
    LogicalAnd, // &&

    // rank 12, left to right
    LogicalOr, // ||

    // rank 13, right to left
    TernaryOperator,

    // rank 14, right to left
    Assignment, // =
    AssignmentSum, // +=
    AssignmentDifference, // -=
    AssignmentProduct, // *=
    AssignmentQuotient, // /=
    AssignmentRemainder, // %=
    AssignmentLeftShift, // <<=
    AssignmentRightShift, // >>=
    AssignmentBitwiseAnd, // &=
    AssignmentBitwiseXor, // ^=
    AssignmentBitwiseOr, // |=

    // rank 15, left to right
    Comma, // ,
};

} // namespace common
} // namespace compiler
