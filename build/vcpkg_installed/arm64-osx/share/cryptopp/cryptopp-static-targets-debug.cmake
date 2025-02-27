#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cryptopp::cryptopp" for configuration "Debug"
set_property(TARGET cryptopp::cryptopp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(cryptopp::cryptopp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libcryptopp.a"
  )

list(APPEND _cmake_import_check_targets cryptopp::cryptopp )
list(APPEND _cmake_import_check_files_for_cryptopp::cryptopp "${_IMPORT_PREFIX}/debug/lib/libcryptopp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
