# - Try to find FreeType
# Once done, this will define
#
#  FREETYPE2_FOUND - system has FreeType2
#  FREETYPE2_INCLUDE_DIRS - the Freetype2 include directories
#  FREETYPE2_LIBRARIES - link these to use Freetype2

# Include dir
find_path(FREETYPE2_INCLUDE_DIR
  NAMES ft2build.h
  PATH_SUFFIXES freetype2
  PATHS $ENV{FREETYPE2_DIR}/include
)


# Finally the library itself
find_library(FREETYPE2_LIBRARIES
  NAMES freetype
  PATHS $ENV{FREETYPE2_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Freetype2 DEFAULT_MSG FREETYPE2_LIBRARIES FREETYPE2_INCLUDE_DIR)
mark_as_advanced( FREETYPE2_INCLUDE_DIR FREETYPE2_LIBRARIES )

