#ifndef PHLEX_ARROW_COMMON_PHLEXSOURCE_HPP
#define PHLEX_ARROW_COMMON_PHLEXSOURCE_HPP

// Implicit-provider helper for phlex::source implementations (the second and
// last choke point on Phlex's experimental API, after PhlexTypes.hpp).
//
// A phlex::source answers create_providers(selector) with zero or more
// phlex::provider_bundle.  The bundle type is stable (`phlex::`), but filling
// it needs Phlex's experimental erased-product API: a product_specification
// built with make_type_id<T>(), and a provider function returning a
// product_ptr made by product_for().  Every phlex-arrow-* / dune-daq source has
// the same "advertise one product of type T, read it per data cell" shape, so
// provide_if_selected() factors that out and keeps the experimental spelling
// in this one header.
//
// The stage: since Phlex 0.4, an implicit provider must name a real stage (the
// reserved "CURRENT" is rejected at graph construction), i.e. the stage that
// produced the data the source reads.  Callers pass it in (typically from a
// `stage` config key).
//
// This header pulls in phlex/source.hpp, so consumers link phlex::source (or
// phlex::core).  The phlex-arrow-common library itself does not include it.

#include "phlex_arrow_common/PhlexTypes.hpp"

#include "phlex/concurrency.hpp"
#include "phlex/source.hpp"

#include <string>
#include <utility>

namespace phlex_arrow {

/// Return the (single) implicit-provider bundle for a product of type T,
/// identified by (creator, suffix, layer, stage), if `selector` selects it;
/// otherwise return no bundles.
///
/// `read` is a copyable callable `T(phlex::data_cell_index const&)` that makes
/// the product for one data cell.
template <typename T, typename Read>
phlex::provider_bundles provide_if_selected(phlex::product_selector const& selector,
                                            std::string const& creator,
                                            std::string const& suffix,
                                            std::string const& layer,
                                            std::string const& stage,
                                            phlex::concurrency conc,
                                            Read read)
{
    product_specification spec{algorithm_name{creator}, identifier{suffix},
                               phlex::experimental::make_type_id<T>()};
    if (!selector.match(spec, identifier{layer}, identifier{stage})) {
        return {};  // this source cannot satisfy the request
    }

    phlex::provider_bundles bundles;
    bundles.push_back(phlex::provider_bundle{
      .provider_function =
        [read = std::move(read)](phlex::data_cell_index const& id) {
            return phlex::experimental::product_for(read(id));
        },
      .max_concurrency = conc,
      .spec = std::move(spec),
      .layer = layer,
      .stage = stage});
    return bundles;
}

}  // namespace phlex_arrow

#endif  // PHLEX_ARROW_COMMON_PHLEXSOURCE_HPP
