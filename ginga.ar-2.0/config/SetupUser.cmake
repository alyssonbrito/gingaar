# Build shared file
set(BUILD_SHARED 0)

set(CMAKE_INSTALL_PREFIX $ENV{DEPLOY_DIR})
set(CMAKE_CONFIG_INSTALL_PREFIX $ENV{DEPLOY_DIR})

LIST(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_INSTALL_PREFIX} )

# Override build type,  Release, Debug, RelWithDebInfo
set(CMAKE_BUILD_TYPE "Release")

set(SILENT 1)

# Set compile tests in true
set(COMPILE_TESTS 0)
# Set run tests in false
set(RUN_TESTS 0)

# Ginga
set(SYS_PLAYER_USE_VLC 0)
if(WIN32)
set(SYS_HTML_USE_CEF 1)
else()
set(SYS_HTML_USE_GTK 1)
endif(WIN32)

# Boost Library
SET(Boost_NO_SYSTEM_PATHS TRUE)
SET(BOOST_ROOT $ENV{DEPLOY_DIR})
include_directories(BEFORE SYSTEM $ENV{DEPLOY_DIR})

# Reset cached LUA_LIBRARIES
UNSET(LUA_LIBRARIES CACHE)
