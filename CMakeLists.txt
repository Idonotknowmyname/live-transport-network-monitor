cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(live-transport-network-monitor CXX)

# Dependencies
# If dependencies are managed with conan, we prepend the current build folder
# to the CMake module path, where CMake looks for dependencies.
# list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(Boost 1.85 REQUIRED COMPONENTS system)


add_library(network-monitor STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/websocket-client.cpp")
target_compile_features(network-monitor PUBLIC cxx_std_14)
target_link_libraries(network-monitor PUBLIC boost::boost)
target_include_directories(network-monitor PUBLIC inc)

add_executable(network-monitor-tests "${CMAKE_CURRENT_SOURCE_DIR}/tests/websocket-client.cpp")
target_compile_features(network-monitor-tests PRIVATE cxx_std_14)
target_link_libraries(network-monitor-tests PRIVATE boost::boost network-monitor)
target_include_directories(network-monitor-tests PRIVATE inc)