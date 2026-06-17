#include "phlex_arrow_common/CoveragePolicy.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace phlex_arrow {

bool ModuleClaim::claims(const std::string& product) const
{
    if (wildcard()) return true;
    return std::find(products.begin(), products.end(), product) != products.end();
}

CoverageReport analyze_coverage(const std::vector<std::string>& requested,
                                const std::vector<ModuleClaim>& claims)
{
    CoverageReport report;
    for (const auto& product : requested) {
        std::vector<std::string> claimants;
        for (const auto& claim : claims) {
            if (claim.claims(product)) claimants.push_back(claim.name);
        }
        if (claimants.empty()) {
            report.unclaimed.push_back(product);
        }
        else if (claimants.size() > 1) {
            report.conflicts.emplace_back(product, std::move(claimants));
        }
    }
    return report;
}

bool CoverageReport::ok(bool allow_redundant) const
{
    if (!unclaimed.empty()) return false;
    if (!allow_redundant && !conflicts.empty()) return false;
    return true;
}

std::string CoverageReport::message(bool allow_redundant) const
{
    if (ok(allow_redundant)) return {};

    std::ostringstream os;
    if (!unclaimed.empty()) {
        os << "output coverage: " << unclaimed.size()
           << " requested product(s) claimed by no output module (would be silently dropped):";
        for (const auto& p : unclaimed) os << " '" << p << "'";
        os << ". Add the product to an output module's claim list, or remove it"
              " from the persist request.";
    }
    if (!allow_redundant && !conflicts.empty()) {
        if (!unclaimed.empty()) os << '\n';
        os << "output coverage: " << conflicts.size()
           << " product(s) claimed by multiple output modules (would be written twice):";
        for (const auto& [product, modules] : conflicts) {
            os << " '" << product << "' <-";
            for (const auto& m : modules) os << " " << m;
            os << ';';
        }
        os << " resolve the overlap, or set allow_redundant to opt into"
              " intentional redundancy.";
    }
    return os.str();
}

void enforce_coverage(const std::vector<std::string>& requested,
                      const std::vector<ModuleClaim>& claims,
                      bool allow_redundant)
{
    auto report = analyze_coverage(requested, claims);
    if (!report.ok(allow_redundant)) {
        throw std::runtime_error(report.message(allow_redundant));
    }
}

}  // namespace phlex_arrow
