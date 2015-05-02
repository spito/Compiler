#pragma once

namespace compiler {
namespace ast {

template< typename ReturnedInformation, typename StoredInformation >
struct Traversal {
    using Information = ReturnedInformation;
    using StoredInfo = StoredInformation;
};

} // namespace ast
} // namespace compiler
