add_executable(test_cell_address test_cell_address.cpp)
target_link_libraries(test_cell_address PRIVATE phlex_arrow_common arrow_hdf::arrow_hdf_core)
add_test(NAME cell_address COMMAND test_cell_address)

add_executable(test_store_address test_store_address.cpp)
target_link_libraries(test_store_address PRIVATE phlex_arrow_common phlex::model ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME store_address COMMAND test_store_address)

add_executable(test_arrow_products test_arrow_products.cpp)
target_link_libraries(test_arrow_products PRIVATE phlex_arrow_common
  Arrow::arrow_shared phlex::model ${PHLEX_ARROW_RUNTIME_LIBS})
add_test(NAME arrow_products COMMAND test_arrow_products)
