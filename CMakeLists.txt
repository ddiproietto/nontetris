cmake_minimum_required (VERSION 2.6)
project (Nontetris)

set (NONTETRIS_VERSION_MAJOR 0)
set (NONTETRIS_VERSION_MINOR 1)

configure_file (
	"${PROJECT_SOURCE_DIR}/NontetrisConfig.h.in"
	"${PROJECT_BINARY_DIR}/NontetrisConfig.h"
	)

include_directories("${PROJECT_BINARY_DIR}")

set(MY_SOURCES nontetris.cpp physichandler.cpp inputhandler.cpp graphichandler.cpp inputhandler.cpp)

if(DEFINED DUETTO)
	set (MY_SOURCES ${MY_SOURCES} duettogl.cpp)
	add_library (nontetris ${MY_SOURCES})
	target_link_libraries(nontetris cxxabi)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else(DEFINED DUETTO)
	#include SOIL library
	set (EXTERNAL_SOURCES SOIL/SOIL.c SOIL/image_DXT.c SOIL/image_helper.c SOIL/stb_image_aug.c)
	add_executable (nontetris ${MY_SOURCES} ${EXTERNAL_SOURCES})
endif(DEFINED DUETTO)


#find_package(Box2D)
#if (BOX2D_FOUND)
#	include_directories(${BOX2D_INCLUDE_DIRS})
#endif (BOX2D_FOUND)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
IF(DEFINED EMSCRIPTEN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
ENDIF(DEFINED EMSCRIPTEN)


###BOX2D INCLUSION
set (BOX2D_INSTALL OFF)
set (BOX2D_INSTALL_DOC OFF)
IF(NOT DEFINED EMSCRIPTEN)
	set (BOX2D_BUILD_SHARED OFF)
	set (BOX2D_BUILD_STATIC ON)
ELSE(NOT DEFINED EMSCRIPTEN)
	set (BOX2D_BUILD_SHARED ON)
	MESSAGE(STATUS "EMSCRIPTEN!!!")
	set (BOX2D_BUILD_STATIC OFF)
ENDIF(NOT DEFINED EMSCRIPTEN)
set (BOX2D_BUILD_EXAMPLES OFF)

include_directories ("${PROJECT_SOURCE_DIR}/Box2D/")
add_subdirectory (Box2D)
target_link_libraries (nontetris Box2D)
###END BOX2D INCLUSION


if(NOT DEFINED DUETTO)
	if(NOT DEFINED EMSCRIPTEN)
	##OpenGL##
	find_package(OpenGL REQUIRED)
	target_link_libraries (nontetris ${OPENGL_LIBRARIES})
	##END OpenGL##
	endif(NOT DEFINED EMSCRIPTEN)

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
endif(NOT DEFINED DUETTO)
