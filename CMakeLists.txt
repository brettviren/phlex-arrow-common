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

# Pure Arrow<->HDF5 serializer (the M-axis). Provides arrow_hdf::arrow_hdf_core
# (Address/Hierarchy) which this package's structured addressing builds upon.
find_package(arrow_hdf CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# phlex_arrow_common — Phlex-aware, technology-agnostic helpers.
#
# Currently: the structured (cell/creator/product) <-> arrow_hdf::Address
# mapping (ddm-c3s.14).  Store->Address extraction from a Phlex product_store,
# Arrow-product selection, and the convert-node helper (ddm-c3s.2) will be
# added here and will pull in Phlex.
# ---------------------------------------------------------------------------
add_library(phlex_arrow_common SHARED
  phlex_arrow_common/CellAddress.cpp
)
target_include_directories(phlex_arrow_common
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(phlex_arrow_common
  PUBLIC
    arrow_hdf::arrow_hdf_core
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
