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

#include "phlex_arrow_common/CellAddress.hpp"
#include "phlex_arrow_common/PhlexTypes.hpp"

#include "phlex/model/data_cell_index.hpp"

#include <string>
#include <vector>

namespace phlex_arrow {

/// Structured address of a product in `store`: the (layer, number) cells from
/// store.index() (outermost first), creator from store.source(), and the given
/// product suffix.  A store at the job root yields no cells.
StructuredAddress store_structured(const phlex_arrow::product_store& store,
                                   const std::string& product);

/// Convenience: the raw path components for a product in `store`
/// (= make_address(store_structured(store, product))).
std::vector<std::string> store_address(const phlex_arrow::product_store& store,
                                       const std::string& product);

// --- read side ------------------------------------------------------------
//
// A reader (a single-provider source) is invoked per data_cell_index and must
// address the data written by some ORIGINAL creator.  Unlike the write side,
// the creator is NOT the reader's own source() — it is the writer's creator
// (supplied by configuration) — so it is passed explicitly here.  The cells are
// taken from the supplied index (the same walk as store_structured uses on
// store.index()); store_structured is the (store, store.source()) special case.

/// Structured address for `product` at data-cell `index`, written by `creator`:
/// the (layer, number) cells from `index` up to (not including) the job root
/// (outermost first), plus `creator` and `product`.
StructuredAddress index_structured(const phlex::data_cell_index& index,
                                   const std::string& creator, const std::string& product);

/// Convenience: the raw path components
/// (= make_address(index_structured(index, creator, product))).
std::vector<std::string> index_address(const phlex::data_cell_index& index,
                                       const std::string& creator, const std::string& product);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_STOREADDRESS_H
