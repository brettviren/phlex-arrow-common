#ifndef PHLEX_ARROW_COMMON_PHLEXTYPES_HPP
#define PHLEX_ARROW_COMMON_PHLEXTYPES_HPP

// Single point of dependency on Phlex's "verboten" model types.
//
// The Phlex 0.3.0 guidance is that any interface whose name contains
// `experimental`, `internal`, or `detail` should not be used directly.  But
// 0.3.0's "Rationalize namespaces" work did NOT promote these core model types
// into the stable top-level `phlex::` namespace — it only shuffled them BETWEEN
// the verboten namespaces, and provides no top-level alias for any of them:
//
//   * product_store, algorithm_name, identifier  -> still phlex::experimental
//   * product_specification                      -> moved to phlex::detail
//
// We therefore cannot avoid the verboten spelling without losing the types.
// Aliasing them here confines that unavoidable dependency to this one header:
// the next namespace move (e.g. experimental/detail -> stable) is a one-file
// edit instead of a sweep across every phlex-arrow-* package.
//
// By contrast, the Phlex types we use that DO live in the stable top-level
// `phlex` namespace — data_cell_index, product_selector (renamed from
// product_query in 0.3.0), concurrency, handle — are used directly, not aliased.
//
// Usage: refer to phlex_arrow::product_store etc. instead of the
// phlex::experimental:: / phlex::detail:: spelling.

#include "phlex/model/algorithm_name.hpp"
#include "phlex/model/identifier.hpp"
#include "phlex/model/product_specification.hpp"
#include "phlex/model/product_store.hpp"

namespace phlex_arrow {

using product_store = phlex::experimental::product_store;
using product_specification = phlex::detail::product_specification;
using algorithm_name = phlex::experimental::algorithm_name;
using identifier = phlex::experimental::identifier;

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_PHLEXTYPES_HPP
