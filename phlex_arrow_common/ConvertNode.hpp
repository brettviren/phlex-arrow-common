#ifndef PHLEX_ARROW_COMMON_CONVERTNODE_H
#define PHLEX_ARROW_COMMON_CONVERTNODE_H

// Generic registration helper for Arrow convert nodes.
//
// Every to_arrow/from_arrow Phlex transform shares the same wiring: declare a
// single-input single-output transform, route the input by a product_selector,
// and name the output product suffix.  register_convert() factors out that
// boilerplate so a domain package (e.g. wire-cell-phlex-arrow) is reduced to a
// list of (name, converter, query, suffix) instantiations.
//
// This header is technology- and domain-agnostic: it carries no knowledge of
// WCT types or of any file technology.  It does, however, pull in the Phlex
// module/graph machinery (phlex/module.hpp), which requires C++23 and links
// phlex::module — so only consumers that build plugins include it.  The
// phlex-arrow-common library itself (C++20, phlex::model only) does not.

#include "phlex/core/product_selector.hpp"
#include "phlex/module.hpp"

#include <string>
#include <utility>

namespace phlex_arrow {

/// Register a Phlex transform node that converts one product to another.
///
/// \param m       The module graph proxy from PHLEX_REGISTER_ALGORITHMS.
/// \param name    The transform's algorithm name.
/// \param conv    A callable of signature `Out(In const&)` (the converter).
/// \param input   The product_selector selecting the single input product.
/// \param suffix  The output product suffix.
/// \param conc    Concurrency (default serial).
///
/// Returns the registration proxy so callers may chain further options.
template <typename Proxy, typename Conv>
auto register_convert(Proxy& m,
                      const std::string& name,
                      Conv&& conv,
                      phlex::product_selector input,
                      const std::string& suffix,
                      phlex::concurrency conc = phlex::concurrency::serial)
{
    return m.transform(name, std::forward<Conv>(conv), conc)
      .input_family(std::move(input))
      .output_product_suffixes(suffix);
}

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_CONVERTNODE_H
