find_path (libfvad_INCLUDE_DIR
    NAMES fvad.h
)
mark_as_advanced (libfvad_INCLUDE_DIR)

find_library (libfvad_LIBRARY NAMES libfvad fvad)
mark_as_advanced (libfvad_LIBRARY)

# Support the REQUIRED and QUIET arguments, and set libfvad_FOUND if found.
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(libfvad
                                  REQUIRED_VARS libfvad_LIBRARY libfvad_INCLUDE_DIR)

if (libfvad_FOUND)
    set (libfvad_LIBRARIES ${libfvad_LIBRARY})
    set (libfvad_INCLUDE_DIRS ${libfvad_INCLUDE_DIR})
else ()
    message (STATUS "No libfvad found")
endif()
