#ifndef PHLEX_ARROW_COMMON_CELLHIERARCHY_HPP
#define PHLEX_ARROW_COMMON_CELLHIERARCHY_HPP

// Interpret a set of stored product addresses as a Phlex cell hierarchy
// (ddm-c3s.15).
//
// arrow-hdf's scan() returns a GENERIC path trie with no cell/creator/product
// meaning (that semantics was deliberately removed from arrow-hdf so it stays a
// reusable technology layer).  The Phlex read side restores it here: given the
// stored product addresses as RAW (unescaped) component lists
//   [layer0, number0, layer1, number1, ..., creator, product]
// (the technology package — e.g. phlex-arrow-hdf — produces these from its
// scan() output + path_unescape, so this header keeps the layering rule: it
// depends only on the standard library and CellAddress, never on arrow-hdf),
// rebuild the (layer, number) cell tree with products grouped per cell, cells
// numerically ordered, and a single layer name validated per depth.

#include "phlex_arrow_common/CellAddress.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace phlex_arrow {

/// A product located at a cell: which creator wrote which product suffix.
struct ProductLoc {
    std::string creator;
    std::string product;
    friend bool operator==(const ProductLoc&, const ProductLoc&) = default;
};

/// One cell in the hierarchy: a (layer, number) level, the products stored AT
/// this cell, and its child cells (numerically sorted by `number`).
struct CellNode {
    std::string layer;
    std::int64_t number{0};
    std::vector<ProductLoc> products;
    std::vector<CellNode> children;
    friend bool operator==(const CellNode&, const CellNode&) = default;
};

/// The whole hierarchy: the layer name at each depth (0 = outermost), the
/// products at the job root (addresses with no cells), and the top-level cells.
struct CellHierarchy {
    std::vector<std::string> layer_names;
    std::vector<ProductLoc> root_products;
    std::vector<CellNode> cells;
    friend bool operator==(const CellHierarchy&, const CellHierarchy&) = default;

    /// Build from product addresses given as RAW component lists.  Each address
    /// is parsed (via parse_address) into (layer, number) cells + creator +
    /// product; the product attaches at its leaf cell (root_products when an
    /// address has no cells).  Cells are deduplicated, their children numeric-
    /// sorted, and products sorted (creator, product) for deterministic output.
    /// Throws std::invalid_argument if two cells at the same depth disagree on
    /// the layer name, or if any address is malformed.
    static CellHierarchy from_addresses(const std::vector<std::vector<std::string>>& addresses);
};

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_CELLHIERARCHY_HPP
