#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::brotli::brotlienc-static" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlienc-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlienc-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "unofficial::brotli::brotlicommon-static"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlienc-static.a"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlienc-static )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlienc-static "${_IMPORT_PREFIX}/debug/lib/libbrotlienc-static.a" )

# Import target "unofficial::brotli::brotlidec-static" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlidec-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlidec-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "unofficial::brotli::brotlicommon-static"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlidec-static.a"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlidec-static )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlidec-static "${_IMPORT_PREFIX}/debug/lib/libbrotlidec-static.a" )

# Import target "unofficial::brotli::brotlicommon-static" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlicommon-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlicommon-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlicommon-static.a"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlicommon-static )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlicommon-static "${_IMPORT_PREFIX}/debug/lib/libbrotlicommon-static.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
