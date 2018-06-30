# - Try to find Lua
# Once done, this will define
#
#  LUA_FOUND - system has Lua
#  LUA_INCLUDE_DIRS - the Lua include directories
#  LUA_LIBRARIES - link these to use Lua

# Include dir
find_path(LUA_INCLUDE_DIR
  NAMES lua.h
  PATH_SUFFIXES inlcude include/lua include/lu5.1 include/lua51
  PATHS $ENV{LUA_DIR}/include
)

# Finally the library itself
find_library(LUA_LIBRARIES
  NAMES lua
  PATHS $ENV{LUA_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DB_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lua51  DEFAULT_MSG  LUA_LIBRARIES LUA_INCLUDE_DIR)

MARK_AS_ADVANCED(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY)

