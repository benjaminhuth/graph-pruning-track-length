cmake_minimum_required(VERSION 3.25)
project(graph_pruning)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# find pybind11 and make a module with binding.cpp
find_package(pybind11 REQUIRED)
set(MODULE_NAME graph_pruning)
pybind11_add_module(${MODULE_NAME} binding.cpp)
target_compile_definitions(${MODULE_NAME} PRIVATE PY_MODULE_NAME=${MODULE_NAME})

add_executable(test test.cpp)
target_compile_definitions(test PRIVATE BOOST_TEST_DYN_LINK BOOST_TEST_MODULE=graph_pruning)
target_link_libraries(test Boost::unit_test_framework)
