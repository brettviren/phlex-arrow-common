// Unit tests for phlex_arrow structured addressing as neutral path components.
#include "phlex_arrow_common/CellAddress.h"

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
    // make_address flattens to ordered components.
    check(make_address({{"run", 3}, {"event", 12}}, "sigproc", "frame")
              == V{"run", "3", "event", "12", "sigproc", "frame"}, "make_address components");
    // job-root (no cells).
    check(make_address({}, "job", "meta") == V{"job", "meta"}, "root components");
    // components are RAW — phlex-arrow-common does NO escaping (that's the tech package's job).
    check(make_address({{"a/b", 5}}, "cr e", "p/q") == V{"a/b", "5", "cr e", "p/q"},
          "components are raw, not escaped");

    // round-trip make -> parse.
    for (const StructuredAddress& s : {
             StructuredAddress{{{"run", 3}, {"event", 12}}, "sigproc", "frame"},
             StructuredAddress{{}, "job", "meta"},
             StructuredAddress{{{"run", 1000000000000LL}}, "c", "p"},
             StructuredAddress{{{"a/b", 5}}, "cr e", "p/q"},
         }) {
        check(parse_address(make_address(s)) == s, "round-trip");
    }

    // parse error cases.
    try { parse_address(V{"only"}); check(false, "need creator+product"); }
    catch (const std::invalid_argument&) {}
    try { parse_address(V{"run", "3", "event", "creator", "product"}); check(false, "unpaired cells"); }
    catch (const std::invalid_argument&) {}
    try { parse_address(V{"run", "notanum", "c", "p"}); check(false, "bad number"); }
    catch (const std::invalid_argument&) {}

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "cell_address OK\n";
    return 0;
}
