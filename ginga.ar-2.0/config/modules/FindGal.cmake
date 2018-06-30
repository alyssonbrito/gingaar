# - Try to find Gal
# Once done, this will define
#
#  GAL_FOUND - system has Gal
#  GAL_INCLUDE_DIRS - the Gal include directories
#  GAL_LIBRARIES - link these to use Gal
################################################################################################################################# 
# There is a warning after including the libgal, as below									#
# warning: libhalgal.so.0, needed by /DEPLOY/LG1210D-gtk3-gles/lib/libgal.so, not found (try using -rpath or -rpath-link)	#
# There is no libhalgal.so.0 in htvbrowser source code, but lib is locates in TV path: ./usr/lib/libhalgal.so			#
#################################################################################################################################

# Include dir
find_path(GAL_INCLUDE_DIR
  NAMES dile_gal.h 
  PATH_SUFFIXES gal dile
  PATHS $ENV{GAL_DIR}/include
)


# Finally the library itself
find_library(GAL_LIBRARIES
  NAMES gal
  PATHS $ENV{GAL_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Gal DEFAULT_MSG GAL_LIBRARIES GAL_INCLUDE_DIR)
mark_as_advanced( GAL_INCLUDE_DIR GAL_LIBRARIES )

