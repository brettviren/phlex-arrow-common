#ifndef PHLEX_ARROW_COMMON_ARROWPRODUCTS_H
#define PHLEX_ARROW_COMMON_ARROWPRODUCTS_H

// Selecting the Arrow-typed products in a Phlex store (ddm-c3s.2, deliverable 2).
//
// Convention: an "Arrow product" is a Phlex product whose value type is
// std::shared_ptr<arrow::Table> (arrow_table_ptr).  The convert nodes
// (ddm-c3s.4) produce products of this type; an Arrow output module consumes
// them.  Fixing the type here gives the write side a single, schema-driven
// product type to handle.

#include "phlex/model/product_specification.hpp"
#include "phlex/model/product_store.hpp"

#include <arrow/api.h>

#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace phlex_arrow {

/// The canonical Phlex product type for an Arrow table.
using arrow_table_ptr = std::shared_ptr<arrow::Table>;

/// True if `type` is the canonical Arrow-table product type.
bool is_arrow_product(const std::type_info& type);

/// The product_specifications in `store` whose product is an Arrow table.
std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store);

/// As above, restricted to products whose suffix is in `suffixes` (the
/// module's configured subset).
std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store,
                      const std::vector<std::string>& suffixes);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_ARROWPRODUCTS_H
