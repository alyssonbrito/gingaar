CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

# Workaround for Bridge issue (XletPlayer is including a file in wrong way)
include_directories("${PROJECT_SOURCE_DIR}/../ncl30-presenter/include")
include_directories("${PROJECT_SOURCE_DIR}/../ncl30/include")
include_directories("$ENV{DEPLOY_DIR}/include/lifia")
# Workaround for Bridge issue (XletPlayer is including a file in wrong way)

Add_Lib (
	PROJECT_NAME "ncl30-presenter"
	PROJECT_DESCRIPTION "This is the NCL Presenter Ginga library." 
	PROJECT_VERSION 0.11.1.0
)

