// Unit test for store_address: product_store -> arrow_hdf::Address (ddm-c3s.2 #1).
#include "phlex_arrow_common/StoreAddress.hpp"
#include "phlex_arrow_common/CellAddress.hpp"

#include "phlex/model/data_cell_index.hpp"
#include "phlex/model/product_store.hpp"
#include "phlex/model/algorithm_name.hpp"

#include <iostream>
#include <string>

using namespace phlex_arrow;
namespace pe = phlex_arrow;  // PhlexTypes.hpp aliases (via StoreAddress.hpp)

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
    // Phlex 0.4: a product_store refers to (does not own) its creator and stage.
    const pe::algorithm_name creator(pe::identifier{"sigproc"}, pe::identifier{"wct_frame_filter"});
    const pe::identifier stage{"sim"};
    pe::product_store store(ev, &creator, &stage);

    auto s = store_structured(store, "frame");
    check((s.cells == std::vector<Cell>{{"run", 1}, {"event", 12}}), "cells extracted outermost-first");
    check(s.product == "frame", "product suffix");
    check(s.stage == std::string(kCurrentStage), "stage normalized to CURRENT");
    check(s.plugin == "sigproc" && s.algorithm == "wct_frame_filter",
          "plugin/algorithm from source accessors");

    // store_address composes via make_address.
    check(store_address(store, "frame")
              == make_address({{"run", 1}, {"event", 12}}, kCurrentStage, "sigproc",
                              "wct_frame_filter", "frame"),
          "store_address matches make_address");
    // sanity: the leading cell components.
    auto comps = store_address(store, "frame");
    check(comps.size() >= 4 && comps[0] == "run" && comps[1] == "1"
              && comps[2] == "event" && comps[3] == "12",
          "components begin with run,1,event,12");

    // Store at the job root -> no cells.
    const pe::algorithm_name job_creator("job");
    pe::product_store jstore(job, &job_creator, &stage);
    auto js = store_structured(jstore, "meta");
    check(js.cells.empty(), "job-root store has no cells");
    check(js.stage == std::string(kCurrentStage) && js.product == "meta",
          "job-root product tail");

    // --- read side: index_address from a data_cell_index + explicit creator ---
    // A reader is handed the cell index (here `ev`) and the writer's creator.
    auto ri = index_structured(*ev, "sigproc:wct_frame_filter", "frame");
    check((ri.cells == std::vector<Cell>{{"run", 1}, {"event", 12}}), "read cells outermost-first");
    check(ri.plugin == "sigproc" && ri.algorithm == "wct_frame_filter" && ri.product == "frame",
          "read creator split into plugin/algorithm");
    // A colon-free creator is the plugin alone.
    check(index_structured(*ev, "input", "frame").plugin == "input"
              && index_structured(*ev, "input", "frame").algorithm == std::string(kEmptyPart),
          "colon-free creator");

    // The read address with the writer's creator reproduces the write address
    // exactly — i.e. a reader rebuilds the same path the writer used.
    check(index_address(*ev, "sigproc:wct_frame_filter", "frame")
              == store_address(store, "frame"),
          "index_address reproduces store_address for the same creator");
    // And it round-trips through parse_address.
    check(parse_address(index_address(*ev, "sigproc:wct_frame_filter", "frame")) == ri,
          "index_address round-trips via parse_address");

    // Read at the job root -> no cells.
    check(index_structured(*job, "job", "meta").cells.empty(), "job-root index has no cells");

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "store_address OK\n";
    return 0;
}
