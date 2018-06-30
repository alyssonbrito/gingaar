CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

include_directories("${PROJECT_SOURCE_DIR}/../ncl30/include")

# Set fb device
setifempty(FB_DEVICE \"/dev/fb0\")

Add_Lib (
	PROJECT_NAME "canvas"
	PROJECT_DESCRIPTION "This is the Canvas library project."
	PROJECT_VERSION 0.0.1
)
