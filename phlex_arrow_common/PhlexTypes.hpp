#ifndef PHLEX_ARROW_COMMON_PHLEXTYPES_HPP
#define PHLEX_ARROW_COMMON_PHLEXTYPES_HPP

// Single point of dependency on Phlex's still-experimental model types.
//
// product_store / product_specification / algorithm_name / identifier live in
// `phlex::experimental` — a PLAIN namespace (no `inline`, and nothing
// re-exports it into top-level `phlex::`).  They are therefore exactly the
// carve-out that the Phlex 0.3.0 "additive / non-breaking EXCEPT code inside
// phlex::experimental" assurance does NOT protect.  Aliasing them here means a
// future namespace move/rename (e.g. experimental -> stable) is a one-file edit
// instead of a sweep across every phlex-arrow-* package.
//
// By contrast, the Phlex types we use that already live in the STABLE top-level
// `phlex` namespace — data_cell_index, product_query, concurrency, handle — are
// covered by the assurance and are used directly (not aliased here).
//
// Usage: refer to phlex_arrow::product_store etc. instead of the
// phlex::experimental:: spelling.

#include "phlex/model/algorithm_name.hpp"
#include "phlex/model/identifier.hpp"
#include "phlex/model/product_specification.hpp"
#include "phlex/model/product_store.hpp"

namespace phlex_arrow {

using product_store = phlex::experimental::product_store;
using product_specification = phlex::experimental::product_specification;
using algorithm_name = phlex::experimental::algorithm_name;
using identifier = phlex::experimental::identifier;

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_PHLEXTYPES_HPP
