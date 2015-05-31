#include "TypeStorage.h"

namespace compiler {
namespace ast {

std::map< std::string, TypeOf > TypeStorage::_namedTypes = {
        { "void", TypeOf( 0, false ) },
        { "char", TypeOf( 1, true ) },
        { "signed char", TypeOf( 1, true ) },
        { "unsigned char", TypeOf( 1, false ) },
        { "short", TypeOf( 2, true ) },
        { "signed short", TypeOf( 2, true ) },
        { "short int", TypeOf( 2, true ) },
        { "signed short int", TypeOf( 2, true ) },
        { "unsigned short", TypeOf( 2, false ) },
        { "unsigned short int", TypeOf( 2, false ) },
        { "int", TypeOf( 4, true ) },
        { "signed", TypeOf( 4, true ) },
        { "signed int", TypeOf( 4, true ) },
        { "unsigned", TypeOf( 4, false ) },
        { "unsigned int", TypeOf( 4, false ) },
        { "long", TypeOf( 8, true ) },
        { "long int", TypeOf( 8, true ) },
        { "signed long", TypeOf( 8, true ) },
        { "signed long int", TypeOf( 8, true ) },
        { "unsigned long", TypeOf( 8, false ) },
        { "unsigned long int", TypeOf( 8, false ) },
};

} // namespace ast
} // namespace compiler
