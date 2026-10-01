// Unit test for the PhlexSource.hpp implicit-provider helper (phlex 0.4 port).
#include "phlex_arrow_common/PhlexSource.hpp"
#include "phlex_arrow_common/TableGroup.hpp"

#include "phlex/model/data_cell_index.hpp"

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++fails; }
}

int main()
{
    auto read = [](phlex::data_cell_index const& id) {
        return phlex_arrow::TableGroup{"wc.frame:" + std::to_string(id.number()), {}};
    };
    // The framework stamps the consumer's argument type onto each selector
    // before calling create_providers(); do the same with the public helper.
    auto provide = [&](phlex::product_selector s) {
        std::vector<phlex::product_selector> sels{std::move(s)};
        phlex::populate_types<std::tuple<phlex_arrow::TableGroup>>(sels);
        auto const& sel = sels.front();
        return phlex_arrow::provide_if_selected<phlex_arrow::TableGroup>(
          sel, "input", "frame", "event", "daq", phlex::concurrency::serial, read);
    };

    auto b = provide(phlex::product_selector{.creator = "input", .layer = "event", .suffix = "frame"});
    check(b.size() == 1, "matching selector -> one bundle");
    if (b.size() == 1) {
        check(b[0].layer == "event" && b[0].stage == "daq", "bundle layer/stage");
        auto ev = phlex::data_cell_index::job()->make_child("event", 7);
        auto prod = b[0].provider_function(*ev);
        check(prod && prod->type() == typeid(phlex_arrow::TableGroup), "product type");
        auto const* tg = static_cast<phlex_arrow::TableGroup const*>(prod->address());
        check(tg->type == "wc.frame:7", "provider reads the requested cell");
    }

    check(provide(phlex::product_selector{.creator = "input", .layer = "event", .suffix = "depos"}).empty(),
          "other suffix -> no bundle");
    check(provide(phlex::product_selector{.layer = "event", .suffix = "frame", .stage = "sim"}).empty(),
          "other stage -> no bundle");
    check(provide(phlex::product_selector{.layer = "run", .suffix = "frame"}).empty(),
          "other layer -> no bundle");
    {
        // A consumer of another type is not served.
        std::vector<phlex::product_selector> sels{{.layer = "event", .suffix = "frame"}};
        phlex::populate_types<std::tuple<int>>(sels);
        check(phlex_arrow::provide_if_selected<phlex_arrow::TableGroup>(
                sels.front(), "input", "frame", "event", "daq", phlex::concurrency::serial, read)
                .empty(),
              "other type -> no bundle");
    }

    if (fails) { std::cerr << fails << " failure(s)\n"; return 1; }
    std::cout << "test_phlex_source: OK\n";
    return 0;
}
