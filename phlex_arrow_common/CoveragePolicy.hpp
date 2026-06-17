#ifndef PHLEX_ARROW_COMMON_COVERAGEPOLICY_H
#define PHLEX_ARROW_COMMON_COVERAGEPOLICY_H

// Output coverage policy (ddm-c3s.8).
//
// In the narrow-waist design every output module receives EVERY store and is
// expected to persist only the products it claims and ignore the rest.  The
// N x M matrix does not vanish — it becomes a CONFIGURATION of which product
// goes to which technology.  Without a policy two failure modes are silent:
//
//   * a product no module claims simply vanishes (silent DROP), and
//   * a product two modules both claim is written twice (silent DOUBLE-WRITE).
//
// This is a pure, technology-agnostic policy over the per-module claim
// declarations gathered from a job's configuration.  It carries no Arrow,
// HDF5, or Phlex types so it can be unit-tested and reused by any output
// technology.  (Per-store runtime logging of intentional drops lives in the
// technology output module, e.g. phlex-arrow-hdf, which knows what it saw.)

#include <string>
#include <utility>
#include <vector>

namespace phlex_arrow {

/// What one output module declares it will persist.
///
/// `products` lists the claimed product suffixes.  An EMPTY list means the
/// module is a WILDCARD (it claims every product offered to it — the default
/// "persist all Arrow products" behaviour).
struct ModuleClaim {
    std::string name;                   // module name, for diagnostics
    std::vector<std::string> products;  // claimed suffixes; empty = wildcard

    bool wildcard() const { return products.empty(); }
    bool claims(const std::string& product) const;
};

/// The outcome of the job-level coverage analysis.
struct CoverageReport {
    /// Requested products that no module claims (silent-drop risk).
    std::vector<std::string> unclaimed;
    /// product -> the (>1) modules that all claim it (double-write risk).
    std::vector<std::pair<std::string, std::vector<std::string>>> conflicts;

    /// Coverage is acceptable when nothing is unclaimed and there are no
    /// conflicts — unless `allow_redundant` opts in to multiple claimants.
    bool ok(bool allow_redundant = false) const;

    /// Human-readable summary of the problems (empty when clean).
    std::string message(bool allow_redundant = false) const;
};

/// Analyze whether `claims` cover exactly the `requested` products.
///
/// `requested` is the set of product suffixes the job intends to persist.  A
/// wildcard module claims every requested product.  A product claimed by more
/// than one module (counting wildcards) is a conflict.
CoverageReport analyze_coverage(const std::vector<std::string>& requested,
                                const std::vector<ModuleClaim>& claims);

/// Fail fast: throws std::runtime_error(report.message()) when coverage is not
/// ok.  Intended to be called once at job-configuration time, before any data
/// flows, so unclaimed/double-claimed products are caught loudly.
void enforce_coverage(const std::vector<std::string>& requested,
                      const std::vector<ModuleClaim>& claims,
                      bool allow_redundant = false);

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_COVERAGEPOLICY_H
