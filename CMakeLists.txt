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

set(HEPH_DEPENDENCY_INCLUDE_DIRS "")
set(HEPH_DEPENDENCY_LIBRARIES "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

file(GLOB_RECURSE HEPH_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    find_package(UUID REQUIRED)
    list(APPEND HEPH_DEPENDENCY_INCLUDE_DIRS ${UUID_INCLUDE_DIRS})
    list(APPEND HEPH_DEPENDENCY_LIBRARIES ${UUID_LIBRARIES})

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
        add_library(Heph STATIC ${HEPH_SOURCES})
    else()
        add_library(Heph SHARED ${HEPH_SOURCES})
    endif()

    target_include_directories(
        Heph PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${HEPH_DEPENDENCY_INCLUDE_DIRS}
    )

    target_link_libraries(Heph PUBLIC ${HEPH_DEPENDENCY_LIBRARIES})

    install(DIRECTORY include/ DESTINATION include)
    install(TARGETS Heph
        EXPORT HephTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    add_custom_target(
        HEPH_EXPORT_HEADERS ALL

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/include

        COMMENT "Copying headers"
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/HephConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/HephConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/HephConfig.cmake"
        INSTALL_DESTINATION lib/cmake/Heph
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/HephConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/HephConfigVersion.cmake"
        DESTINATION lib/cmake/Heph
    )

    install(EXPORT HephTargets
        FILE HephTargets.cmake
        NAMESPACE Heph::
        DESTINATION lib/cmake/Heph
    )

endif()