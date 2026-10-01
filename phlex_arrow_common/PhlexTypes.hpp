#ifndef PHLEX_ARROW_COMMON_PHLEXTYPES_HPP
#define PHLEX_ARROW_COMMON_PHLEXTYPES_HPP

// Single point of dependency on Phlex's non-stable model types.
//
// Phlex has three namespace tiers: `phlex::` (stable), `phlex::experimental::`
// (public but "may change before it is promoted to the stable phlex
// namespace"), and `phlex::detail::` / `internal` (private).
//
// As of Phlex 0.4.1 the model types this package needs are all in the
// experimental tier; none is in `phlex::detail` any more:
//
//   * product_store, algorithm_name, identifier  -> phlex::experimental
//   * product_specification                      -> phlex::experimental
//     (was phlex::detail in 0.3.x; moved by Phlex PR #891)
//
// There is no stable spelling for any of them, so they are aliased here.  That
// confines the experimental dependency to this one header: the next namespace
// move (experimental -> stable) is a one-file edit instead of a sweep across
// every phlex-arrow-* package.  The implicit-provider (phlex::source) helpers
// that need the experimental erased-product API live in the sibling choke
// point PhlexSource.hpp.
//
// By contrast, the Phlex types we use that DO live in the stable top-level
// `phlex` namespace -- data_cell_index, product_selector, concurrency, handle,
// source, provider_bundle -- are used directly, not aliased.
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
