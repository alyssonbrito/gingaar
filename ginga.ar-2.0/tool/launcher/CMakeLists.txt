CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

# Workaround, to eliminate -rdynamic from the linker
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

find_package(Git)
if(GIT_FOUND)
	message("*********************************")
	message("git found: ${GIT_EXECUTABLE}")
	message("*********************************")
endif()

GET_GIT_TAG_AND_REV ( TAG_VERSION BUILD_VERSION )

message("")
message("*****************************************************************")
message("> TAG VERSION   : ${TAG_VERSION}")
message("> BUILD VERSION : ${BUILD_VERSION}")
message("*****************************************************************")
message("")


configure_file(
	"${PROJECT_SOURCE_DIR}/configuration.h.in"
	"${PROJECT_SOURCE_DIR}/include/configuration.h"
)




# Workaround for Bridge issue (XletPlayer is including a file in wrong way)
include_directories("$ENV{DEPLOY_DIR}/include/lifia/ncl30")
include_directories("$ENV{DEPLOY_DIR}/include/lifia")


# Setting location for Configuration Header file
include_directories("${PROJECT_SOURCE_DIR}/include")

# It's necessary to send keys to GTK+ 3.0 and WebOS.
include_directories("$ENV{DEPLOY_DIR}/include/gtk-3.0")


set(CMAKE_C_FLAGS "-std=c99")


#-----------------------------------------------------#
#   Executable should be renamed to "ginga_ncl.bin"   #
#                                                     #
# 'PROJECT_NAME' would be the executable filename.    #
# I should check if there's any MACRO available to    #
# rename this file.                                   #
#-----------------------------------------------------#

Add_Bin (
	PROJECT_NAME "ginga_ncl.bin"
	PROJECT_VERSION 1.0.0
	PROJECT_DESCRIPTION "Launcher for LGE DTV"
	PACKAGE_NAME "Launcher"
	SUBSYSTEM Windows
	BIN_DESTINATION bin
)

INSTALL( DIRECTORY ../ginga/fonts/
        DESTINATION share/ginga/font
        FILES_MATCHING PATTERN "*.ttf"
                PATTERN "*svn*" EXCLUDE
)





#set(CMAKE_C_FLAGS "-std=c99 -I$ENV{DEPLOY_DIR}/include -I$ENV{DEPLOY_DIR}/include/lifia -I$ENV{DEPLOY_DIR}/include/lifia/pem-tv-agent")
#set(CMAKE_CXX_FLAGS "-I$ENV{DEPLOY_DIR}/include -I$ENV{DEPLOY_DIR}/include/lifia -I$ENV{DEPLOY_DIR}/include/lifia/pem-tv-agent")

#set(CMAKE_EXE_LINKER_FLAGS "-L$ENV{DEPLOY_DIR}/lib -L$ENV{DEPLOY_DIR}/lib/lifia -ldtvpem-tv-agent") 
#set(CMAKE_EXE_LINKER_FLAGS "-pthread") 



