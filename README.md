# phlex-arrow-common

Phlex-aware, **technology-agnostic** helpers shared by all `phlex-arrow-<tech>`
packages, in the Phlex narrow-waist file-I/O design
(`phlex-file-io-design.md` §5–6). It sits between the pure `arrow-<tech>`
serializers (e.g. `arrow-hdf`) and the thin per-technology Phlex packages.

Implemented so far:

- **Structured addressing** (`phlex_arrow_common/CellAddress.h`, ddm-c3s.14):
  the `(layer, number)` cells + `creator` + `product` structure, mapped to a
  neutral ordered list of **path components** (`std::vector<std::string>`).
  `make_address(...)` flattens to components; `parse_address(...)` is the
  inverse. Dependency-free (no Phlex, and — deliberately — no technology
  package such as `arrow-hdf`). A technology package turns the components into
  its concrete path (e.g. `arrow_hdf::Address(components)`).
- **Store→address / product selection** (`StoreAddress.h`, `ArrowProducts.h`,
  ddm-c3s.2): extract a `StructuredAddress`/components from a Phlex
  `product_store` (the full `data_cell_index`, not a flattened key), and select
  the Arrow-typed products (`arrow_table_ptr = std::shared_ptr<arrow::Table>`).

Planned: a convert-node helper for `to_arrow`/`from_arrow` transform nodes
(ddm-c3s.2 #3, to be designed with its first consumer `wire-cell-phlex-arrow`).
Read-side driver/provider helpers (ddm-c3s.3) are deferred until Phlex ≥ 0.3.0.

Dependencies: Phlex (`product_store`, `data_cell_index`) and Apache Arrow (the
narrow-waist intermediate). NOT any technology serializer.

## Build

Requires the Spack view (`/devel/ddm/local`: Phlex, Arrow); pins the Spack
GCC 15 toolchain via `$CC`/`$CXX`:

```bash
export CC="$(spack -e wcph location -i gcc@15)/bin/gcc"
export CXX="$(spack -e wcph location -i gcc@15)/bin/g++"
cmake --preset default -S source/phlex-arrow-common -B builds/phlex-arrow-common
cmake --build builds/phlex-arrow-common
ctest --test-dir builds/phlex-arrow-common
```
