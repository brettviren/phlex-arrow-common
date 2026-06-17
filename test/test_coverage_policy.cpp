// Unit test for the output coverage policy (ddm-c3s.8).

#include "phlex_arrow_common/CoveragePolicy.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using phlex_arrow::analyze_coverage;
using phlex_arrow::enforce_coverage;
using phlex_arrow::ModuleClaim;

static int fails = 0;
static void check(bool ok, const std::string& what)
{
    std::cout << (ok ? "ok   " : "FAIL ") << what << "\n";
    if (!ok) ++fails;
}

int main()
{
    const std::vector<std::string> requested{"frame", "deposet", "tensorset"};

    // Full, disjoint coverage -> ok, no problems.
    {
        std::vector<ModuleClaim> claims{
          {"hdf_a", {"frame", "deposet"}},
          {"hdf_b", {"tensorset"}},
        };
        auto r = analyze_coverage(requested, claims);
        check(r.ok(), "full coverage ok");
        check(r.unclaimed.empty() && r.conflicts.empty(), "full coverage clean");
        check(r.message().empty(), "clean message empty");
    }

    // Unclaimed product -> not ok, named in message, enforce throws.
    {
        std::vector<ModuleClaim> claims{{"hdf", {"frame", "deposet"}}};
        auto r = analyze_coverage(requested, claims);
        check(!r.ok(), "unclaimed -> not ok");
        check(r.unclaimed.size() == 1 && r.unclaimed[0] == "tensorset", "unclaimed identifies tensorset");
        check(r.message().find("tensorset") != std::string::npos, "message names unclaimed product");

        bool threw = false;
        try { enforce_coverage(requested, claims); }
        catch (const std::runtime_error&) { threw = true; }
        check(threw, "enforce throws on unclaimed");
    }

    // Double-claim -> reported; not ok by default, ok only with opt-in.
    {
        std::vector<ModuleClaim> claims{
          {"hdf_a", {"frame", "deposet"}},
          {"hdf_b", {"frame", "tensorset"}},  // also claims frame
        };
        auto r = analyze_coverage(requested, claims);
        check(!r.ok(/*allow_redundant=*/false), "double-claim not ok by default");
        check(r.ok(/*allow_redundant=*/true), "double-claim ok with opt-in");
        check(r.conflicts.size() == 1 && r.conflicts[0].first == "frame", "conflict identifies frame");
        check(r.conflicts[0].second.size() == 2, "conflict lists both modules");
        check(r.message().find("frame") != std::string::npos, "conflict message names product");

        bool threw = false;
        try { enforce_coverage(requested, claims, /*allow_redundant=*/false); }
        catch (const std::runtime_error&) { threw = true; }
        check(threw, "enforce throws on double-claim by default");

        bool threw2 = false;
        try { enforce_coverage(requested, claims, /*allow_redundant=*/true); }
        catch (const std::runtime_error&) { threw2 = true; }
        check(!threw2, "enforce passes double-claim with opt-in");
    }

    // Wildcard module claims everything -> full coverage.
    {
        std::vector<ModuleClaim> claims{{"hdf_all", {}}};  // empty = wildcard
        auto r = analyze_coverage(requested, claims);
        check(r.ok(), "wildcard covers all");
        check(r.unclaimed.empty(), "wildcard leaves nothing unclaimed");
    }

    // Two wildcards -> every product double-claimed.
    {
        std::vector<ModuleClaim> claims{{"hdf_x", {}}, {"hdf_y", {}}};
        auto r = analyze_coverage(requested, claims);
        check(!r.ok(false), "two wildcards conflict");
        check(r.conflicts.size() == requested.size(), "every product conflicts under two wildcards");
    }

    // Wildcard + specific -> specific product double-claimed by both.
    {
        std::vector<ModuleClaim> claims{{"hdf_all", {}}, {"hdf_frame", {"frame"}}};
        auto r = analyze_coverage(requested, claims);
        check(!r.ok(false), "wildcard+specific conflicts");
        check(r.conflicts.size() == 1 && r.conflicts[0].first == "frame", "only the specific product conflicts");
    }

    // Empty request -> trivially ok.
    {
        auto r = analyze_coverage({}, {{"hdf", {"frame"}}});
        check(r.ok(), "empty request ok");
    }

    std::cout << (fails ? "COVERAGE POLICY FAILURES\n" : "all coverage policy checks OK\n");
    return fails ? 1 : 0;
}
