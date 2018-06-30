# Set warning level
# To set CXXFLAGS which is from environment
# Add "-pthread" here as a workaround for a bug in gtest:
#     https://groups.google.com/group/googletestframework/browse_thread/thread/d00d069043a2f22b#
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wall -Wextra -Wno-unused-function -pthread")

# Set debug flags
#set(CMAKE_CXX_FLAGS_DEBUG "-rdynamic")

# This macro adds the executable for the System
MACRO(add_system_executable PROJECT_NAME PROJECT_NAME_SOURCE_FILES)
    	  ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME_SOURCE_FILES}})
ENDMACRO(add_system_executable)
