cmake_minimum_required(VERSION 3.28)

project(HephDocs VERSION ${HEPH_VERSION} LANGUAGES CXX)

find_package(Doxygen)
if(DOXYGEN_FOUND)

    get_filename_component(HEPH_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

    set(HEPH_DOCS_MAINPAGE "${HEPH_ROOT_DIR}/README.md")
    set(HEPH_DOCS_INPUT "${HEPH_ROOT_DIR}/README.md ${HEPH_ROOT_DIR}/include")
    set(HEPH_DOCS_PREDEFINED "HEPH_VERSION=\"v${HEPH_VERSION}\" HEPH_VERSION_MAJOR=${HEPH_VERSION_MAJOR} HEPH_VERSION_MINOR=${HEPH_VERSION_MINOR} HEPH_VERSION_PATCH=${HEPH_VERSION_PATCH}")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/Doxyfile"
        "${CMAKE_BINARY_DIR}/docs/Doxyfile"
        @ONLY
    )

    execute_process(
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs
    )

else()

    message(WARNING "Doxygen not found, documentation will not be generated.")

endif()