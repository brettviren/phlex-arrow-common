// Unit test for store_address: product_store -> arrow_hdf::Address (ddm-c3s.2 #1).
#include "phlex_arrow_common/StoreAddress.hpp"
#include "phlex_arrow_common/CellAddress.hpp"

#include "phlex/model/data_cell_index.hpp"
#include "phlex/model/product_store.hpp"
#include "phlex/model/algorithm_name.hpp"

#include <iostream>
#include <string>

using namespace phlex_arrow;
namespace pe = phlex::experimental;

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++fails; }
}

int main()
{
    // /run/1/event/12 cell, creator "sigproc".
    auto job = phlex::data_cell_index::job();
    auto run = job->make_child("run", 1);
    auto ev = run->make_child("event", 12);
    pe::product_store store(ev, pe::algorithm_name("sigproc"));

    auto s = store_structured(store, "frame");
    check((s.cells == std::vector<Cell>{{"run", 1}, {"event", 12}}), "cells extracted outermost-first");
    check(s.product == "frame", "product suffix");
    check(s.creator == store.source().full(), "creator from source");

    // store_address composes via make_address.
    check(store_address(store, "frame")
              == make_address({{"run", 1}, {"event", 12}}, store.source().full(), "frame"),
          "store_address matches make_address");
    // sanity: the leading cell components.
    auto comps = store_address(store, "frame");
    check(comps.size() >= 4 && comps[0] == "run" && comps[1] == "1"
              && comps[2] == "event" && comps[3] == "12",
          "components begin with run,1,event,12");

    // Store at the job root -> no cells.
    pe::product_store jstore(job, pe::algorithm_name("job"));
    auto js = store_structured(jstore, "meta");
    check(js.cells.empty(), "job-root store has no cells");
    check(store_address(jstore, "meta")
              == make_address({}, jstore.source().full(), "meta"),
          "job-root store_address");

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "store_address OK\n";
    return 0;
}
