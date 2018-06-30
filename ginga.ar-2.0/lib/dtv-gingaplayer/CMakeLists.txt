CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

SetIfEmpty( GINGA_CONNECTOR_SOCKET \"/tmp/gingaconnector.socket\" )

# Workaround for Bridge issue (XletPlayer is including a file in wrong way)
include_directories("${PROJECT_SOURCE_DIR}/../ncl30-presenter/include")
include_directories("${PROJECT_SOURCE_DIR}/../ncl30/include")
include_directories("$ENV{DEPLOY_DIR}/include/lifia")
# Workaround for Bridge issue (XletPlayer is including a file in wrong way)

Add_Lib (
	PROJECT_NAME "gingaplayer"
	PROJECT_DESCRIPTION "This is the Ginga Player library project."
	PROJECT_VERSION 0.0.1
)


