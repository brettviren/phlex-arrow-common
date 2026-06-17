add_executable(test_cell_address test_cell_address.cpp)
target_link_libraries(test_cell_address PRIVATE phlex_arrow_common)
add_test(NAME cell_address COMMAND test_cell_address)

add_executable(test_store_address test_store_address.cpp)
target_link_libraries(test_store_address PRIVATE phlex_arrow_common phlex::model ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME store_address COMMAND test_store_address)

add_executable(test_arrow_products test_arrow_products.cpp)
target_link_libraries(test_arrow_products PRIVATE phlex_arrow_common
  Arrow::arrow_shared phlex::model ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME arrow_products COMMAND test_arrow_products)

# Output coverage policy (ddm-c3s.8): pure std, no Arrow/Phlex needed.
add_executable(test_coverage_policy test_coverage_policy.cpp)
target_link_libraries(test_coverage_policy PRIVATE phlex_arrow_common
  ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME coverage_policy COMMAND test_coverage_policy)

# Cell-hierarchy interpretation (ddm-c3s.15): pure std + CellAddress.
add_executable(test_cell_hierarchy test_cell_hierarchy.cpp)
target_link_libraries(test_cell_hierarchy PRIVATE phlex_arrow_common
  ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME cell_hierarchy COMMAND test_cell_hierarchy)
