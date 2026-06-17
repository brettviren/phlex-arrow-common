#ifndef PHLEX_ARROW_COMMON_CELLADDRESS_H
#define PHLEX_ARROW_COMMON_CELLADDRESS_H

// Structured (cell / creator / product) addressing mapped onto the generic
// arrow_hdf::Address (a flat '/'-delimited path).
//
// arrow-hdf stays domain/framework-neutral; this Phlex-glue helper owns the
// STRUCTURE: a sequence of (layer_name, number) cells (mirroring a Phlex
// data_cell_index), plus a creator and a product name.  It builds the flat
// path components and hands them to arrow_hdf::Address (which escapes/joins),
// and parses a path back into the structured form.
//
// This header depends only on arrow-hdf (arrow_hdf::Address) and the standard
// library — not on Phlex itself.  Extracting the cells/creator/product FROM a
// Phlex product_store is a separate, Phlex-touching helper (see ddm-c3s.2).

#include "arrow_hdf/Address.h"

#include <cstdint>
#include <string>
#include <vector>

namespace phlex_arrow {

/// One hierarchy level: a named layer and its cell number.
struct Cell {
    std::string layer;
    std::int64_t number;
    friend bool operator==(const Cell&, const Cell&) = default;
};

/// A structured address: cells (outermost first) + creator + product.
struct StructuredAddress {
    std::vector<Cell> cells;
    std::string creator;
    std::string product;
    friend bool operator==(const StructuredAddress&, const StructuredAddress&) = default;
};

/// Build a flat arrow_hdf::Address from structured parts.  The path components
/// are [layer0, number0, layer1, number1, ..., creator, product]; arrow-hdf
/// escapes and joins them.  Empty `cells` yields "/<creator>/<product>".
arrow_hdf::Address make_address(const std::vector<Cell>& cells,
                                const std::string& creator, const std::string& product);
arrow_hdf::Address make_address(const StructuredAddress& addr);

/// Parse a flat arrow_hdf::Address back into structured form (the inverse of
/// make_address): the trailing two components are creator and product; the
/// preceding components must form (layer, number) pairs.  Throws
/// std::invalid_argument if the path does not have that shape.
StructuredAddress parse_address(const arrow_hdf::Address& addr);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_CELLADDRESS_H
