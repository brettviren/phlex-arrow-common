#ifndef PHLEX_ARROW_COMMON_ARROWPRODUCTS_H
#define PHLEX_ARROW_COMMON_ARROWPRODUCTS_H

// Selecting the Arrow products in a Phlex store (ddm-c3s.2, deliverable 2).
//
// The canonical Phlex Arrow product is phlex_arrow::TableGroup (see
// TableGroup.h).  An output module selects products of that type and persists
// each via a technology package.

#include "phlex_arrow_common/TableGroup.hpp"

#include "phlex/model/product_specification.hpp"
#include "phlex/model/product_store.hpp"

#include <string>
#include <typeinfo>
#include <vector>

namespace phlex_arrow {

/// True if `type` is the canonical Arrow product type (TableGroup).
bool is_arrow_product(const std::type_info& type);

/// The product_specifications in `store` whose product is a TableGroup.
std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store);

/// As above, restricted to products whose suffix is in `suffixes` (the
/// module's configured subset).
std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store,
                      const std::vector<std::string>& suffixes);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_ARROWPRODUCTS_H
