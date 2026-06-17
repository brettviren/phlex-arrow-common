#ifndef PHLEX_ARROW_COMMON_TABLEGROUP_H
#define PHLEX_ARROW_COMMON_TABLEGROUP_H

// TableGroup: the canonical Phlex Arrow product — a typed, named group of Arrow
// tables.  This is the single uniform type that flows through the graph for
// BOTH atomic WCT types (one member) and aggregates (several members), so an
// IFrame->Arrow convert node has a single output port.
//
//   type    : the product type marker, e.g. "wc.frame", "wc.deposet".
//   members : reserved-name -> table.  By convention (owned by the domain glue
//             wire-cell-phlex-arrow, NOT arrow-hdf):
//               wc.frame     -> { traces, frame_tags, trace_tags, cmm }
//               wc.deposet   -> { deposet }
//               wc.tensorset -> { tensorset }
//
// Depends only on Apache Arrow + std (no Phlex, no technology serializer).
// The technology packages (phlex-arrow-hdf, ...) translate a TableGroup into
// primitive arrow_hdf calls (arrow_hdf::write_tables iterates the members);
// arrow-hdf itself never sees this type.

#include <arrow/api.h>

#include <map>
#include <memory>
#include <string>

namespace phlex_arrow {

/// A single Arrow table (a TableGroup member, and the type the convert nodes
/// hand around for one table).
using arrow_table_ptr = std::shared_ptr<arrow::Table>;

struct TableGroup {
    std::string type;                                  ///< product type marker
    std::map<std::string, arrow_table_ptr> members;    ///< reserved name -> table

    friend bool operator==(const TableGroup&, const TableGroup&) = default;
};

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_TABLEGROUP_H
