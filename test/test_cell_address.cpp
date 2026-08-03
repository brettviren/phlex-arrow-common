// Unit tests for phlex_arrow structured addressing as neutral path components.
#include "phlex_arrow_common/CellAddress.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace phlex_arrow;
using V = std::vector<std::string>;

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++fails; }
}

int main()
{
    // make_address flattens to ordered components: the fixed 4-part
    // stage/plugin/algorithm/product tail follows the cell pairs.
    check(make_address({{"run", 3}, {"event", 12}}, "CURRENT", "sigproc", "wct_frame_filter",
                       "frame")
              == V{"run", "3", "event", "12", "CURRENT", "sigproc", "wct_frame_filter", "frame"},
          "make_address components");
    // job-root (no cells).
    check(make_address({}, "CURRENT", "job", "boot", "meta")
              == V{"CURRENT", "job", "boot", "meta"}, "root components");
    // empty-part normalization: stage -> kCurrentStage, others -> kEmptyPart.
    check(make_address({}, "", "p", "", "") == V{kCurrentStage, "p", kEmptyPart, kEmptyPart},
          "empty parts normalized");
    // components are RAW — phlex-arrow-common does NO escaping (that's the tech package's job).
    check(make_address({{"a/b", 5}}, "s t", "cr e", "al g", "p/q")
              == V{"a/b", "5", "s t", "cr e", "al g", "p/q"},
          "components are raw, not escaped");

    // round-trip make -> parse.
    for (const StructuredAddress& s : {
             StructuredAddress{{{"run", 3}, {"event", 12}}, "CURRENT", "sigproc",
                               "wct_frame_filter", "frame"},
             StructuredAddress{{}, "CURRENT", "job", "boot", "meta"},
             StructuredAddress{{{"run", 1000000000000LL}}, "S", "c", "a", "p"},
             StructuredAddress{{{"a/b", 5}}, "s t", "cr e", "al g", "p/q"},
         }) {
        check(parse_address(make_address(s)) == s, "round-trip");
    }

    // parse error cases.
    try { parse_address(V{"s", "c", "p"}); check(false, "need 4-part tail"); }
    catch (const std::invalid_argument&) {}
    try {
        parse_address(V{"run", "3", "event", "stage", "creator", "algo", "product"});
        check(false, "unpaired cells");
    }
    catch (const std::invalid_argument&) {}
    try { parse_address(V{"run", "notanum", "s", "c", "a", "p"}); check(false, "bad number"); }
    catch (const std::invalid_argument&) {}

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "cell_address OK\n";
    return 0;
}
