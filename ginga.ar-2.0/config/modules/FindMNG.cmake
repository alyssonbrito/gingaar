# - Find mng
# Find the native MNG headers and libraries.
#
#  MNG_INCLUDE_DIRS - where to find libmng.h, etc.
#  MNG_LIBRARIES    - List of libraries when using mng.
#  MNG_FOUND        - True if mng found.


# Look for the header file.
FIND_PATH(MNG_INCLUDE_DIR NAMES libmng.h
	PATHS $ENV{MNG_DIR}/include
)
MARK_AS_ADVANCED(MNG_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(MNG_LIBRARY NAMES
    mng
  # Windows MSVC prebuilts:
	PATHS $ENV{MNG_DIR}/lib $ENV{MNG_DIR}/bin
)
MARK_AS_ADVANCED(MNG_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set MNG_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MNG DEFAULT_MSG MNG_LIBRARY MNG_INCLUDE_DIR)

IF(MNG_FOUND)
  SET(MNG_LIBRARIES ${MNG_LIBRARY})
  SET(MNG_INCLUDE_DIRS ${MNG_INCLUDE_DIR})
ENDIF(MNG_FOUND)
