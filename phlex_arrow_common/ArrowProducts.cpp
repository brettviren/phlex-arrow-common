#include "phlex_arrow_common/ArrowProducts.h"

#include <algorithm>
#include <string_view>

namespace phlex_arrow {

bool is_arrow_product(const std::type_info& type)
{
    return type == typeid(TableGroup);
}

std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store)
{
    std::vector<phlex::experimental::product_specification> out;
    for (const auto& [spec, prod] : store) {
        if (prod && is_arrow_product(prod->type())) out.push_back(spec);
    }
    return out;
}

std::vector<phlex::experimental::product_specification>
select_arrow_products(const phlex::experimental::product_store& store,
                      const std::vector<std::string>& suffixes)
{
    std::vector<phlex::experimental::product_specification> out;
    for (const auto& spec : select_arrow_products(store)) {
        const std::string suffix{static_cast<std::string_view>(spec.suffix())};
        if (std::find(suffixes.begin(), suffixes.end(), suffix) != suffixes.end()) {
            out.push_back(spec);
        }
    }
    return out;
}

}  // namespace phlex_arrow
