find_path(UUID_INCLUDE_DIR
  NAMES uuid/uuid.h uuid.h
  PATHS
    /usr/include
    /usr/local/include
    $ENV{UUID_INCLUDE_DIR}
)

find_library(UUID_LIBRARY
  NAMES uuid
  PATHS
    /usr/lib
    /usr/local/lib
    $ENV{UUID_LIBRARY}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UUID
  REQUIRED_VARS UUID_INCLUDE_DIR UUID_LIBRARY
  VERSION_VAR UUID_VERSION
)

if(UUID_FOUND)
  set(UUID_INCLUDE_DIRS ${UUID_INCLUDE_DIR})
  set(UUID_LIBRARIES ${UUID_LIBRARY})
else()
  set(UUID_INCLUDE_DIRS "")
  set(UUID_LIBRARIES "")
endif()

mark_as_advanced(UUID_INCLUDE_DIR UUID_LIBRARY)