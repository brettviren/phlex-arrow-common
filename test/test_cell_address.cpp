// Unit tests for phlex_arrow structured (cell/creator/product) addressing.
#include "phlex_arrow_common/CellAddress.h"

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
    // make_address builds the flat path.
    {
        auto a = make_address({{"run", 3}, {"event", 12}}, "sigproc", "frame");
        check(a.path() == "/run/3/event/12/sigproc/frame", "make_address path: " + a.path());
    }
    // job-root (no cells).
    {
        auto a = make_address({}, "job", "meta");
        check(a.path() == "/job/meta", "root make_address: " + a.path());
    }
    // components needing escaping round-trip through parse.
    {
        StructuredAddress s{{{"a/b", 5}}, "cr e", "p/q"};
        auto a = make_address(s);
        check(a.path() == "/a%2Fb/5/cr%20e/p%2Fq", "escaped path: " + a.path());
        check(parse_address(a) == s, "structured round-trip (escaped)");
    }
    // round-trip for several shapes.
    for (const StructuredAddress& s : {
             StructuredAddress{{{"run", 3}, {"event", 12}}, "sigproc", "frame"},
             StructuredAddress{{}, "job", "meta"},
             StructuredAddress{{{"run", 1000000000000LL}}, "c", "p"},
         }) {
        check(parse_address(make_address(s)) == s, "round-trip: " + make_address(s).path());
    }
    // parse error cases.
    try { parse_address(arrow_hdf::Address(std::string("/only"))); check(false, "need creator+product"); }
    catch (const std::invalid_argument&) {}
    try { parse_address(arrow_hdf::Address(std::string("/run/3/event/creator/product"))); check(false, "unpaired cells"); }
    catch (const std::invalid_argument&) {}
    try { parse_address(arrow_hdf::Address(std::string("/run/notanum/creator/product"))); check(false, "bad number"); }
    catch (const std::invalid_argument&) {}

    if (fails) { std::cerr << fails << " failures\n"; return 1; }
    std::cout << "cell_address OK\n";
    return 0;
}
