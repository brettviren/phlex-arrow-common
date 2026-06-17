#ifndef PHLEX_ARROW_COMMON_STOREADDRESS_H
#define PHLEX_ARROW_COMMON_STOREADDRESS_H

// Map a Phlex product_store + product name to an arrow_hdf::Address.
//
// This is the Phlex-touching half of the structured addressing (ddm-c3s.2,
// deliverable 1): it reads the full structured data_cell_index directly (the
// (layer_name, number) chain), the store source (creator), and the product
// suffix, and builds the address via the neutral make_address(). Reading the
// whole index — rather than a flattened key — is what fixes the FORM
// index-flattening defect.

#include "phlex_arrow_common/CellAddress.h"

#include "phlex/model/product_store.hpp"

#include <string>

namespace phlex_arrow {

/// Extract the structured address of a product in `store`: the (layer, number)
/// cells from store.index() (outermost first), creator from store.source(), and
/// the given product suffix.  A store at the job root yields no cells.
StructuredAddress store_structured(const phlex::experimental::product_store& store,
                                   const std::string& product);

/// Convenience: the flat arrow_hdf::Address for a product in `store`.
arrow_hdf::Address store_address(const phlex::experimental::product_store& store,
                                 const std::string& product);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_STOREADDRESS_H
