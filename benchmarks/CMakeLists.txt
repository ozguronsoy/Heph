cmake_minimum_required(VERSION 3.28)

project(HephBenchmarks VERSION ${HEPH_VERSION} LANGUAGES CXX)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable Google Benchmark's tests" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Google Benchmark's tests" FORCE)

include(FetchContent)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.5
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(benchmark)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${HEPH_DEPENDENCY_INCLUDE_DIRS}
)

file(GLOB_RECURSE HEPH_BENCHMARK_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

add_executable(
    HephBenchmarks
    ${HEPH_BENCHMARK_SOURCES}
    ${HEPH_SOURCES}
)

target_link_libraries(
    HephBenchmarks PRIVATE 
    ${HEPH_DEPENDENCY_LIBRARIES}
    benchmark::benchmark
)