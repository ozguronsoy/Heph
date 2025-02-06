cmake_minimum_required(VERSION 3.28)

project(HephTests VERSION ${HEPH_VERSION} LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(googletest)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}/
)

file(GLOB_RECURSE HEPH_TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

add_executable(
    HephTests
    ${HEPH_TEST_SOURCES}
    ${HEPH_SOURCES}
)

target_link_libraries(
    HephTests
    ${HEPH_LIBRARIES}
    gtest
    gtest_main
)