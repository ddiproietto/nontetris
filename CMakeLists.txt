cmake_minimum_required (VERSION 2.6)
project (Nontetris)

set (NONTETRIS_VERSION_MAJOR 0)
set (NONTETRIS_VERSION_MINOR 1)

configure_file (
	"${PROJECT_SOURCE_DIR}/NontetrisConfig.h.in"
	"${PROJECT_BINARY_DIR}/NontetrisConfig.h"
	)

include_directories("${PROJECT_BINARY_DIR}")

add_executable (nontetris nontetris.cpp physichandler.cpp graphichandler.cpp)

#find_package(Box2D)
#if (BOX2D_FOUND)
#	include_directories(${BOX2D_INCLUDE_DIRS})
#endif (BOX2D_FOUND)

###BOX2D INCLUSION
set (BOX2D_INSTALL OFF)
set (BOX2D_INSTALL_DOC OFF)
set (BOX2D_BUILD_SHARED OFF)
set (BOX2D_BUILD_STATIC ON)
set (BOX2D_BUILD_EXAMPLES OFF)

include_directories ("${PROJECT_SOURCE_DIR}/Box2D/")
add_subdirectory (Box2D)
target_link_libraries (nontetris Box2D)
###END BOX2D INCLUSION

##GLFW##
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED libglfw)
target_link_libraries(nontetris ${GLFW_LIBRARIES})
##END GLFW##

##GLEW##
find_package(PkgConfig REQUIRED)
pkg_search_module(GLEW REQUIRED glew)
target_link_libraries(nontetris ${GLEW_LIBRARIES})
##END GLEW##

add_definitions("-std=c++11 -g")
