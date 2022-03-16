cmake_minimum_required(VERSION 3.14)
project(SystemCSafety)

set (CMAKE_CXX_STANDARD 20)

find_library(SYSTEMC_LIBRARY 
    NAMES systemc
    PATHS /opt/systemc/lib/
    PATHS /opt/systemc/lib-linux64/
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SYSTEMC_INCLUDE /opt/systemc/include) 
set(SYSTEMC_AMS_INCLUDE /opt/systemc-ams/include) 

add_executable(dram-fta-example
    examples/dram-fta-example.cpp
)

target_include_directories(dram-fta-example
    PRIVATE ${SYSTEMC_INCLUDE}
)

target_link_libraries(dram-fta-example
    PRIVATE ${SYSTEMC_LIBRARY}
)

add_executable(dram-metrics-example
    examples/dram-metrics-example.cpp
)

target_include_directories(dram-metrics-example
    PRIVATE ${SYSTEMC_INCLUDE}
)

target_link_libraries(dram-metrics-example
    PRIVATE ${SYSTEMC_LIBRARY}
)



enable_testing()

add_executable(
  tests
  tests/tests.cpp
)
target_link_libraries(tests
  gtest_main
  ${SYSTEMC_LIBRARY}
)

target_include_directories(tests
    PRIVATE ${SYSTEMC_INCLUDE}
)

include(GoogleTest)
gtest_discover_tests(tests)