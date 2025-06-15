cmake_minimum_required(VERSION 3.28)

# extract version from file
file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" HEPH_VERSION)
string(STRIP "${HEPH_VERSION}" HEPH_VERSION)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" HEPH_VERSION_REGEX "${HEPH_VERSION}")
set(HEPH_VERSION_MAJOR ${CMAKE_MATCH_1})
set(HEPH_VERSION_MINOR ${CMAKE_MATCH_2})
set(HEPH_VERSION_PATCH ${CMAKE_MATCH_3})

add_definitions(
    -DHEPH_VERSION="v${HEPH_VERSION}"
    -DHEPH_VERSION_MAJOR=${HEPH_VERSION_MAJOR}
    -DHEPH_VERSION_MINOR=${HEPH_VERSION_MINOR}
    -DHEPH_VERSION_PATCH=${HEPH_VERSION_PATCH}
)

option(HEPH_BUILD_TESTS "HEPH_BUILD_TESTS" Off)
option(HEPH_BUILD_DOCS "HEPH_BUILD_DOCS" Off)
option(HEPH_BUILD_STATIC "HEPH_BUILD_STATIC" Off)
option(HEPH_BUILD_SHARED "HEPH_BUILD_SHARED" Off)

project(Heph VERSION ${HEPH_VERSION} LANGUAGES CXX)

if(NOT DEFINED CMAKE_CXX_STANDARD)

    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

else()

    if(CMAKE_CXX_STANDARD LESS 23)
        message(FATAL_ERROR "The C++ version must be 23 or higher.")
    endif()

endif()

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

file(GLOB_RECURSE HEPH_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

set(HEPH_LIBRARIES)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(HEPH_LIBRARIES ${HEPH_LIBRARIES} uuid)
endif()

if(HEPH_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(HEPH_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if(HEPH_BUILD_STATIC OR HEPH_BUILD_SHARED)

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_IMPORT_LIBRARY_PREFIX "")
        set(CMAKE_IMPORT_LIBRARY_SUFFIX ".lib")
        set(CMAKE_STATIC_LIBRARY_PREFIX "")
        set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
    endif()

    add_definitions(-DHEPH_SHARED_LIB -DHEPH_EXPORTS)
    
    if(HEPH_BUILD_STATIC)
        add_library(${CMAKE_PROJECT_NAME} STATIC ${HEPH_SOURCES})
    else()
        add_library(${CMAKE_PROJECT_NAME} SHARED ${HEPH_SOURCES})
    endif()

    target_include_directories(
        ${CMAKE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(${CMAKE_PROJECT_NAME} ${HEPH_LIBRARIES})

    add_custom_target(
        HEPH_EXPORT_HEADERS ALL

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/include/Heph

        COMMENT "Copying all headers with preserved subfolders"
    )

endif()