CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE($ENV{DEPOT}/config/Tvd.cmake)

include_directories("${PROJECT_SOURCE_DIR}/include")

# Enabling Colors for native logging
set(CMAKE_C_FLAGS "-std=c99 -DPEM_NATIVE_LOG_COLOR_ON -fms-extensions -fPIC")

Add_Lib (
	PROJECT_NAME "pem-tv-util"
	PROJECT_DESCRIPTION "Presentation Engine Utilities. It should be used by PEM-TV-AGENT and LAUNCHER" 
	PROJECT_VERSION 0.0.1
)

