# phlex-arrow-common

Phlex-aware, **technology-agnostic** helpers shared by all `phlex-arrow-<tech>`
packages, in the Phlex narrow-waist file-I/O design
(`phlex-file-io-design.md` §5–6). It sits between the pure `arrow-<tech>`
serializers (e.g. `arrow-hdf`) and the thin per-technology Phlex packages.

Implemented so far:

- **Structured addressing** (`phlex_arrow_common/CellAddress.h`, ddm-c3s.14):
  the `(layer, number)` cells + `creator` + `product` structure (the concept
  factored out of `arrow-hdf` to keep it neutral), mapped onto a generic
  `arrow_hdf::Address`. `make_address(...)` builds the flat path components;
  `parse_address(...)` is the inverse. Depends only on `arrow-hdf` (no Phlex).

Planned (ddm-c3s.2): store→address extraction from a Phlex `product_store`,
Arrow-product selection, and a convert-node helper for `to_arrow`/`from_arrow`
transform nodes (these pull in Phlex). Read-side driver/provider helpers
(ddm-c3s.3) are deferred until Phlex ≥ 0.3.0.

## Build

Requires `arrow-hdf` installed (dev prefix `/devel/ddm/install`) and the Spack
view (`/devel/ddm/local`); pins the Spack GCC 15 toolchain via `$CC`/`$CXX`:

```bash
export CC="$(spack -e wcph location -i gcc@15)/bin/gcc"
export CXX="$(spack -e wcph location -i gcc@15)/bin/g++"
cmake --preset default -S source/phlex-arrow-common -B builds/phlex-arrow-common
cmake --build builds/phlex-arrow-common
ctest --test-dir builds/phlex-arrow-common
```
