#include "phlex_arrow_common/StoreAddress.hpp"

#include "phlex/model/algorithm_name.hpp"
#include "phlex/model/data_cell_index.hpp"
#include "phlex/model/identifier.hpp"

#include <algorithm>
#include <string_view>
#include <tuple>

namespace phlex_arrow {

StructuredAddress index_structured(const phlex::data_cell_index& index,
                                   const std::string& creator, const std::string& product)
{
    StructuredAddress s;
    s.stage = kCurrentStage;
    std::tie(s.plugin, s.algorithm) = split_creator(creator);
    s.product = product;

    auto push = [&s](const phlex::data_cell_index& idx) {
        auto sv = static_cast<std::string_view>(idx.layer_name());
        s.cells.push_back(Cell{std::string(sv), static_cast<std::int64_t>(idx.number())});
    };

    // Record this cell and each ancestor that has a parent — i.e. all but the
    // job root — innermost first, then reverse to outermost-first.
    if (index.has_parent()) {
        push(index);
        for (auto idx = index.parent(); idx && idx->has_parent(); idx = idx->parent()) {
            push(*idx);
        }
    }
    std::reverse(s.cells.begin(), s.cells.end());
    return s;
}

std::vector<std::string> index_address(const phlex::data_cell_index& index,
                                       const std::string& creator, const std::string& product)
{
    return make_address(index_structured(index, creator, product));
}

StructuredAddress store_structured(const phlex_arrow::product_store& store,
                                   const std::string& product)
{
    // The write-side special case: cells from store.index(), creator from the
    // store's own source().
    const auto& source = store.source();
    const auto plugin = std::string(static_cast<std::string_view>(source.plugin()));
    const auto algorithm = std::string(static_cast<std::string_view>(source.algorithm()));

    auto idx = store.index();
    StructuredAddress s;
    if (idx) {
        s = index_structured(*idx, plugin + ":" + algorithm, product);
    }
    else {
        s.stage = kCurrentStage;
        s.product = product;
    }
    // Set directly from the accessors (a plugin label could itself contain a
    // colon; the split above is only for the config-string convenience form).
    s.plugin = plugin;
    s.algorithm = algorithm;
    return s;
}

std::vector<std::string> store_address(const phlex_arrow::product_store& store,
                                       const std::string& product)
{
    return make_address(store_structured(store, product));
}

}  // namespace phlex_arrow
