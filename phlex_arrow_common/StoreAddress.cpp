#include "phlex_arrow_common/StoreAddress.h"

#include "phlex/model/algorithm_name.hpp"
#include "phlex/model/data_cell_index.hpp"
#include "phlex/model/identifier.hpp"

#include <algorithm>
#include <string_view>

namespace phlex_arrow {

StructuredAddress store_structured(const phlex::experimental::product_store& store,
                                   const std::string& product)
{
    StructuredAddress s;
    s.creator = store.source().full();
    s.product = product;

    // Walk the data_cell_index chain from this cell up to (but not including)
    // the parent-less job root, recording (layer, number).  Innermost first;
    // reverse to outermost-first.
    for (auto idx = store.index(); idx && idx->has_parent(); idx = idx->parent()) {
        auto sv = static_cast<std::string_view>(idx->layer_name());
        s.cells.push_back(Cell{std::string(sv), static_cast<std::int64_t>(idx->number())});
    }
    std::reverse(s.cells.begin(), s.cells.end());
    return s;
}

arrow_hdf::Address store_address(const phlex::experimental::product_store& store,
                                 const std::string& product)
{
    return make_address(store_structured(store, product));
}

}  // namespace phlex_arrow
