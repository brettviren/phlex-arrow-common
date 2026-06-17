// Unit test for CellHierarchy::from_addresses (ddm-c3s.15).

#include "phlex_arrow_common/CellAddress.hpp"
#include "phlex_arrow_common/CellHierarchy.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace phlex_arrow;

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++fails; }
}

int main()
{
    // A small file: two events under run 1 (numbers out of order), one event
    // carrying two products from two creators, plus a job-root product.
    std::vector<std::vector<std::string>> addrs{
      make_address({{"run", 1}, {"event", 12}}, "sigproc", "frame"),
      make_address({{"run", 1}, {"event", 3}}, "sigproc", "frame"),
      make_address({{"run", 1}, {"event", 12}}, "sim", "deposet"),
      make_address({}, "job", "meta"),  // root product (no cells)
    };

    auto h = CellHierarchy::from_addresses(addrs);

    // Layer names per depth.
    check((h.layer_names == std::vector<std::string>{"run", "event"}), "layer_names per depth");

    // Root product.
    check((h.root_products == std::vector<ProductLoc>{{"job", "meta"}}), "root product captured");

    // One top-level cell: run 1.
    check(h.cells.size() == 1 && h.cells[0].layer == "run" && h.cells[0].number == 1, "top cell run/1");

    // Its children: event 3 then event 12 (NUMERIC sort, not lexicographic).
    const auto& evs = h.cells[0].children;
    check(evs.size() == 2, "two event cells");
    check(evs[0].layer == "event" && evs[0].number == 3, "event 3 sorts first");
    check(evs[1].number == 12, "event 12 sorts second (numeric, not '12' < '3')");

    // event 3: one product; event 12: two products grouped, sorted (creator,product).
    check((evs[0].products == std::vector<ProductLoc>{{"sigproc", "frame"}}), "event 3 product");
    check((evs[1].products == std::vector<ProductLoc>{{"sigproc", "frame"}, {"sim", "deposet"}}),
          "event 12 two products grouped + sorted");

    // Leaf cells have no children.
    check(evs[0].children.empty() && evs[1].children.empty(), "event cells are leaves");

    // Inconsistent layer name at the same depth -> throws.
    bool threw = false;
    try {
        CellHierarchy::from_addresses({
          make_address({{"run", 1}}, "a", "x"),
          make_address({{"crate", 1}}, "b", "y"),  // depth 0 disagrees: run vs crate
        });
    }
    catch (const std::invalid_argument&) { threw = true; }
    check(threw, "inconsistent layer name throws");

    // Empty input -> empty hierarchy.
    auto empty = CellHierarchy::from_addresses({});
    check(empty.cells.empty() && empty.root_products.empty() && empty.layer_names.empty(),
          "empty input yields empty hierarchy");

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "cell_hierarchy OK\n";
    return 0;
}
