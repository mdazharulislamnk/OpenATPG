cmake_minimum_required(VERSION 3.14)
project(OpenATPG CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files (excluding tests)
set(SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/netlist.cpp
    src/simulator.cpp
    src/atpg.cpp
    src/visualizer.cpp
)

# Main executable
add_executable(atpg_engine src/main.cpp ${SOURCES})
target_include_directories(atpg_engine PRIVATE src)

# Optional GraphViz dot check
find_program(DOT_EXECUTABLE dot)
if(DOT_EXECUTABLE)
    message(STATUS "Graphviz dot found: ${DOT_EXECUTABLE}")
else()
    message(WARNING "Graphviz dot not found. Visualizer output can't be rendered automatically.")
endif()

# Tests setup with GTest via FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(atpg_tests tests/test_main.cpp ${SOURCES})
target_include_directories(atpg_tests PRIVATE src)
target_link_libraries(atpg_tests gtest_main)

include(GoogleTest)
gtest_discover_tests(atpg_tests)
