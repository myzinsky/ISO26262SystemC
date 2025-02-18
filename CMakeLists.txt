cmake_minimum_required(VERSION 3.14)
project(SystemCSafety)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ Standard")

FetchContent_Declare(
  systemc
  GIT_REPOSITORY https://github.com/accellera-official/systemc.git
  GIT_TAG 3.0.0)

set(DISABLE_COPYRIGHT_MESSAGE True)
FetchContent_MakeAvailable(systemc)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library
add_library(iso26262systemc INTERFACE)
add_library(iso26262systemc::iso26262systemc ALIAS iso26262systemc)

target_include_directories(iso26262systemc
    INTERFACE ${CMAKE_SOURCE_DIR}
)

# Examples
add_executable(dram-fta-example examples/dram-fta-example.cpp)
target_link_libraries(dram-fta-example PRIVATE SystemC::systemc iso26262systemc)

add_executable(dram-metrics-example examples/dram-metrics-example.cpp)
target_link_libraries(dram-metrics-example PRIVATE SystemC::systemc iso26262systemc)

add_executable(dram-metrics-refactored examples/dram-metrics-refactored.cpp)
target_link_libraries(dram-metrics-refactored PRIVATE SystemC::systemc iso26262systemc)

# Testing
enable_testing()

add_executable(tests tests/tests.cpp)
target_link_libraries(tests gtest_main SystemC::systemc iso26262systemc)

include(GoogleTest)
gtest_discover_tests(tests)
