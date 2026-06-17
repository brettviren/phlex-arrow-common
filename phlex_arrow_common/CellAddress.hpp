#ifndef PHLEX_ARROW_COMMON_CELLADDRESS_H
#define PHLEX_ARROW_COMMON_CELLADDRESS_H

// Structured (cell / creator / product) addressing expressed as a neutral
// ordered list of path components (std::vector<std::string>).
//
// A structured address is a sequence of (layer_name, number) cells (mirroring a
// Phlex data_cell_index), plus a creator and a product name.  This maps to the
// flat component list
//   [layer0, number0, layer1, number1, ..., creator, product]
// and back.  The components are RAW (not escaped): turning them into a concrete
// technology path (escaping/joining) is the technology package's job
// (e.g. phlex-arrow-hdf builds an arrow_hdf::Address from these components).
//
// This header therefore depends only on the standard library — not on Phlex
// and not on any technology package (arrow-hdf, ...).  Keeping the
// tech-agnostic glue free of a specific-technology dependency is deliberate.

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

/// Flatten structured parts to ordered path components:
///   [layer0, number0, ..., creator, product]  (raw, not escaped).
/// Empty `cells` yields {creator, product}.
std::vector<std::string> make_address(const std::vector<Cell>& cells,
                                      const std::string& creator, const std::string& product);
std::vector<std::string> make_address(const StructuredAddress& addr);

/// Inverse of make_address: interpret raw path components back into structured
/// form.  The trailing two components are creator and product; the preceding
/// components must form (layer, number) pairs.  Throws std::invalid_argument if
/// the components do not have that shape.
StructuredAddress parse_address(const std::vector<std::string>& components);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_CELLADDRESS_H
