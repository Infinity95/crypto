

find_path(AntTweakBar_INCLUDE_DIR
    NAMES
        AntTweakBar.h
    HINTS
        ${AntTweakBar_LOCATION}
    PATHS
        ${CMAKE_SOURCE_DIR}/AntTweakBar/include
)

find_library(AntTweakBar_LIBRARY
    NAMES
        AntTweakBar
    HINTS
        ${AntTweakBar_LOCATION}
    PATHS
        ${CMAKE_SOURCE_DIR}/AntTweakBar/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AntTweakBar DEFAULT_MSG AntTweakBar_LIBRARY AntTweakBar_INCLUDE_DIR)

mark_as_advanced(AntTweakBar_LIBRARY AntTweakBar_INCLUDE_DIR)

set(AntTweakBar_LIBRARIES ${AntTweakBar_LIBRARY})
set(AntTweakBar_INCLUDE_DIRS ${AntTweakBar_INCLUDE_DIR})