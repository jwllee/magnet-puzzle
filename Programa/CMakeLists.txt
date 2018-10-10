cmake_minimum_required(VERSION 2.8.4)
project(solver C)

set(CMAKE_C_STANDARD 11)

set(ROOT ./src)
set(PROJECT_SOURCE_DIR ./src)

include_directories("${PROJECT_SOURCE_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/solver")
add_subdirectory(src/solver)

# test files
add_executable(test-simple tests/solver/simple.c)
target_link_libraries(test-simple Solver)
target_link_libraries(test-simple SolverUtils)

add_executable(test-utils tests/solver/utils.c)
target_link_libraries(test-utils Solver)
target_link_libraries(test-utils SolverUtils)
