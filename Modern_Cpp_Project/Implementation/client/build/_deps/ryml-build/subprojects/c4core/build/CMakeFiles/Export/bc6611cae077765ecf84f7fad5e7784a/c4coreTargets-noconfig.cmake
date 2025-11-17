#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "c4core::c4core" for configuration ""
set_property(TARGET c4core::c4core APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(c4core::c4core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libc4core.a"
  )

list(APPEND _cmake_import_check_targets c4core::c4core )
list(APPEND _cmake_import_check_files_for_c4core::c4core "${_IMPORT_PREFIX}/lib/libc4core.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
