#ifndef PHLEX_ARROW_COMMON_CELLADDRESS_H
#define PHLEX_ARROW_COMMON_CELLADDRESS_H

// Structured (cell / product-identity) addressing expressed as a neutral
// ordered list of path components (std::vector<std::string>).
//
// A structured address is a sequence of (layer_name, number) cells (mirroring a
// Phlex data_cell_index), plus the FOUR product-identity parts (the "address
// scheme v2" of ddm-c3s.20):
//
//   stage      processing stage (phlex product_selector::stage); an unset
//              stage is normalized to the reserved default "CURRENT"
//   plugin     module instance label (algorithm_name::plugin())
//   algorithm  node name within the module (algorithm_name::algorithm())
//   product    product suffix; an empty suffix is normalized to "_"
//
// (The product's layer is NOT repeated: it is the innermost cell.)  This maps
// to the flat component list
//   [layer0, number0, layer1, number1, ..., stage, plugin, algorithm, product]
// and back; the FIXED four-component tail makes the boundary between address
// and any payload sub-paths deterministic.  Files carrying such addresses
// stamp the convention in a root attribute (kAddressSchemeAttr below) so
// readers/scanners parse by declared convention.  The components are RAW (not escaped): turning them into a concrete
// technology path (escaping/joining) is the technology package's job
// (e.g. phlex-arrow-hdf builds an arrow_hdf::Address from these components).
//
// This header therefore depends only on the standard library — not on Phlex
// and not on any technology package (arrow-hdf, ...).  Keeping the
// tech-agnostic glue free of a specific-technology dependency is deliberate.

#include <cstdint>
#include <utility>
#include <string>
#include <vector>

namespace phlex_arrow {

/// One hierarchy level: a named layer and its cell number.
struct Cell {
    std::string layer;
    std::int64_t number;
    friend bool operator==(const Cell&, const Cell&) = default;
};

/// The address-scheme convention stamp: technology packages write
/// kAddressScheme under the file-root attribute kAddressSchemeAttr; readers
/// accept a matching stamp (or its absence, best-effort for legacy files) and
/// reject a mismatched one.
inline constexpr char kAddressSchemeAttr[] = "phlex.address.scheme";
inline constexpr char kAddressScheme[] = "stage-plugin-algorithm-suffix/v1";

/// Normalizations applied by make_address (see the header comment).
inline constexpr char kCurrentStage[] = "CURRENT";
inline constexpr char kEmptyPart[] = "_";

/// A structured address: cells (outermost first) + the product identity.
struct StructuredAddress {
    std::vector<Cell> cells;
    std::string stage;
    std::string plugin;
    std::string algorithm;
    std::string product;
    friend bool operator==(const StructuredAddress&, const StructuredAddress&) = default;
};

/// Flatten structured parts to ordered path components:
///   [layer0, number0, ..., stage, plugin, algorithm, product]  (raw, not
/// escaped).  Empty `cells` yields the four-component tail alone.  An empty
/// stage becomes kCurrentStage; any other empty part becomes kEmptyPart.
std::vector<std::string> make_address(const std::vector<Cell>& cells, const std::string& stage,
                                      const std::string& plugin, const std::string& algorithm,
                                      const std::string& product);
std::vector<std::string> make_address(const StructuredAddress& addr);

/// Split a conventional creator string "plugin:algorithm" at the FIRST colon;
/// a colon-free string is the plugin alone with algorithm kEmptyPart.
std::pair<std::string, std::string> split_creator(const std::string& creator);

/// The canonical "plugin:algorithm" form of a creator string (the identity
/// form used by ProductLoc and read-side configuration).
std::string canonical_creator(const std::string& creator);

/// Inverse of make_address: interpret raw path components back into structured
/// form.  The trailing FOUR components are stage, plugin, algorithm and
/// product; the preceding components must form (layer, number) pairs.  Throws
/// std::invalid_argument if the components do not have that shape.
StructuredAddress parse_address(const std::vector<std::string>& components);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_CELLADDRESS_H
