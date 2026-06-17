cmake_minimum_required(VERSION 3.25)
project(phlex_arrow_common VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "14.1")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15")
      add_compile_options(-Wno-stringop-overflow)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16")
      add_compile_options(-Wno-array-bounds)
    endif()
  endif()
endif()

add_compile_options(-Wall -Wpedantic -Werror)

# Apache Arrow is the narrow-waist intermediate type (needed to identify
# Arrow-typed products).  Note: this package does NOT depend on any specific
# technology serializer (arrow-hdf, ...) — it deals in neutral path components
# (std::vector<std::string>); a technology package turns those into its address.
find_package(Arrow CONFIG REQUIRED)

# Phlex: this package is Phlex-aware (product_store, data_cell_index).
# phlex's exported targets reference Boost::boost and TBB but its Config does
# not find_dependency them, so find them first.
find_package(TBB REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(phlex REQUIRED)

# When using a Spack view, the Phlex shared libs are built with a newer GCC
# runtime than the system compiler; link the view's libstdc++ explicitly so
# GLIBCXX/CXXABI versioned symbols resolve (same pattern as wire-cell-arrow).
set(PHLEX_ARROW_RUNTIME_LIBS "")
foreach(_prefix IN LISTS CMAKE_PREFIX_PATH)
  if(EXISTS "${_prefix}/lib/libstdc++.so.6")
    file(REAL_PATH "${_prefix}/lib/libstdc++.so.6" _stdc_real)
    get_filename_component(_stdc_dir "${_stdc_real}" DIRECTORY)
    set(PHLEX_ARROW_RUNTIME_LIBS "${_stdc_real}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -Wl,-rpath,${_stdc_dir}")
    break()
  endif()
endforeach()

# ---------------------------------------------------------------------------
# phlex_arrow_common — Phlex-aware, technology-agnostic helpers.
#
# - CellAddress: structured (cell/creator/product) <-> path-component
#   (std::vector<std::string>) mapping (ddm-c3s.14), dependency-free.
# - StoreAddress: extract that structure from a Phlex product_store (ddm-c3s.2 #1).
# Arrow-product selection and the convert-node helper (ddm-c3s.2 #2,#3) follow.
# ---------------------------------------------------------------------------
add_library(phlex_arrow_common SHARED
  phlex_arrow_common/CellAddress.cpp
  phlex_arrow_common/StoreAddress.cpp
  phlex_arrow_common/ArrowProducts.cpp
)
target_include_directories(phlex_arrow_common
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(phlex_arrow_common
  PUBLIC
    Arrow::arrow_shared
    phlex::model
    ${PHLEX_ARROW_RUNTIME_LIBS}
)

enable_testing()
add_subdirectory(test)

include(GNUInstallDirs)
install(TARGETS phlex_arrow_common
  EXPORT phlex_arrow_commonTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(DIRECTORY phlex_arrow_common/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/phlex_arrow_common"
  FILES_MATCHING PATTERN "*.h"
)

install(EXPORT phlex_arrow_commonTargets
  FILE phlex_arrow_commonTargets.cmake
  NAMESPACE phlex_arrow_common::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/phlex_arrow_common"
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/phlex_arrow_commonConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/phlex_arrow_commonConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/phlex_arrow_commonConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/phlex_arrow_common"
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/phlex_arrow_commonConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/phlex_arrow_commonConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/phlex_arrow_common"
)
