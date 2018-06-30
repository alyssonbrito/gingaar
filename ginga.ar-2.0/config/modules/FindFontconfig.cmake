# - Try to find Font Config
# Once done, this will define
#
#  FONTCONFIG_FOUND - system has Font Config
#  FONTCONFIG_INCLUDE_DIRS - the Font Config include directories
#  FONTCONFIG_LIBRARIES - link these to use Font Config

# Include dir
find_path(FONTCONFIG_INCLUDE_DIR
  NAMES fontconfig/fontconfig.h 
  PATH_SUFFIXES fontconfig
  PATHS $ENV{FONTCONFIG_DIR}/include
)


# Finally the library itself
find_library(FONTCONFIG_LIBRARIES
  NAMES fontconfig
  PATHS $ENV{FONTCONFIG_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Fontconfig DEFAULT_MSG FONTCONFIG_LIBRARIES FONTCONFIG_INCLUDE_DIR)
mark_as_advanced( FONTCONFIG_INCLUDE_DIR FONTCONFIG_LIBRARIES )

