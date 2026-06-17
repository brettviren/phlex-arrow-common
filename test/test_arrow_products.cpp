// Unit test for Arrow-product selection (ddm-c3s.2 #2): selects TableGroup products.
#include "phlex_arrow_common/ArrowProducts.h"
#include "phlex_arrow_common/TableGroup.h"

#include "phlex/model/data_cell_index.hpp"
#include "phlex/model/product_store.hpp"
#include "phlex/model/algorithm_name.hpp"
#include "phlex/model/product_specification.hpp"

#include <iostream>
#include <string>
#include <string_view>

using namespace phlex_arrow;
namespace pe = phlex::experimental;

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++fails; }
}

int main()
{
    pe::product_store store(phlex::data_cell_index::job(), pe::algorithm_name("sim"));
    store.add_product<TableGroup>(pe::product_specification("frame"), TableGroup{"wc.frame", {}});
    store.add_product<TableGroup>(pe::product_specification("depos"), TableGroup{"wc.deposet", {}});
    store.add_product<int>(pe::product_specification("count"), 5);

    auto sel = select_arrow_products(store);
    check(sel.size() == 2, "two TableGroup products selected (not the int)");
    for (const auto& s : sel) {
        const std::string suf{static_cast<std::string_view>(s.suffix())};
        check(suf == "frame" || suf == "depos", "selected suffix is an Arrow one: " + suf);
    }

    check(select_arrow_products(store, {"frame"}).size() == 1, "filter to frame");
    check(select_arrow_products(store, {"count"}).empty(), "int 'count' not an Arrow product");
    check(select_arrow_products(store, {"nope"}).empty(), "unknown suffix -> none");

    check(is_arrow_product(typeid(TableGroup)), "is_arrow_product true for TableGroup");
    check(!is_arrow_product(typeid(arrow_table_ptr)), "single table is NOT the product type");
    check(!is_arrow_product(typeid(int)), "is_arrow_product false for int");

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "arrow_products OK\n";
    return 0;
}
