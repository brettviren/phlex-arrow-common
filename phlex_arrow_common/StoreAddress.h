#ifndef PHLEX_ARROW_COMMON_STOREADDRESS_H
#define PHLEX_ARROW_COMMON_STOREADDRESS_H

// Map a Phlex product_store + product name to a structured address / neutral
// path components (ddm-c3s.2, deliverable 1).
//
// Reads the FULL structured data_cell_index directly (the (layer_name, number)
// chain) — not a flattened key — which is what fixes the FORM index-flattening
// defect.  The result is technology-neutral: a StructuredAddress, or the raw
// path components.  A technology package turns the components into a concrete
// path (e.g. arrow_hdf::Address); this package does not depend on any.

#include "phlex_arrow_common/CellAddress.h"

#include "phlex/model/product_store.hpp"

#include <string>
#include <vector>

namespace phlex_arrow {

/// Structured address of a product in `store`: the (layer, number) cells from
/// store.index() (outermost first), creator from store.source(), and the given
/// product suffix.  A store at the job root yields no cells.
StructuredAddress store_structured(const phlex::experimental::product_store& store,
                                   const std::string& product);

/// Convenience: the raw path components for a product in `store`
/// (= make_address(store_structured(store, product))).
std::vector<std::string> store_address(const phlex::experimental::product_store& store,
                                       const std::string& product);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_STOREADDRESS_H
