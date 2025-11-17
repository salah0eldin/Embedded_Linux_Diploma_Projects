#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ryml::ryml" for configuration ""
set_property(TARGET ryml::ryml APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(ryml::ryml PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libryml.a"
  )

list(APPEND _cmake_import_check_targets ryml::ryml )
list(APPEND _cmake_import_check_files_for_ryml::ryml "${_IMPORT_PREFIX}/lib/libryml.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
