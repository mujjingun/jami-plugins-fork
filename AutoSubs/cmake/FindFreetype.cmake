find_path (FREETYPE_INCLUDE_DIR
    NAMES ft2build.h
)
mark_as_advanced (FREETYPE_INCLUDE_DIR)

find_library (FREETYPE_LIBRARY NAMES freetype libfreetype)
mark_as_advanced (FREETYPE_LIBRARY)

# Support the REQUIRED and QUIET arguments, and set libfvad_FOUND if found.
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(FREETYPE
                                  REQUIRED_VARS FREETYPE_LIBRARY FREETYPE_INCLUDE_DIR)

if (FREETYPE_FOUND)
    set (FREETYPE_LIBRARIES ${FREETYPE_LIBRARY})
    set (FREETYPE_INCLUDE_DIRS ${FREETYPE_INCLUDE_DIR})
else ()
    message (STATUS "No Freetype found")
endif()
