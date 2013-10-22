cmake_minimum_required (VERSION 2.8)
project (Nontetris)

set (NONTETRIS_VERSION_MAJOR 0)
set (NONTETRIS_VERSION_MINOR 1)

option(USE_GLFW_3 "Use glfw3(otherwise use glfw2)" OFF)

if(USE_GLFW_3)
set(USE_GLFW_VERSION 3)
else(USE_GLFW_3)
set(USE_GLFW_VERSION 2)
endif(USE_GLFW_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
IF(DEFINED EMSCRIPTEN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
ENDIF(DEFINED EMSCRIPTEN)

IF(DEFINED DUETTO)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
ENDIF(DEFINED DUETTO)

if(NOT DEFINED DUETTO)
	if(NOT DEFINED EMSCRIPTEN)
		##OpenGL##
		find_package(OpenGL REQUIRED)
		##END OpenGL##
	endif(NOT DEFINED EMSCRIPTEN)

	##GLFW##
	find_package(PkgConfig REQUIRED)
	if(USE_GLFW_3)
		pkg_search_module(GLFW REQUIRED glfw3)
	else(USE_GLFW_3)
		pkg_search_module(GLFW REQUIRED libglfw)
	endif(USE_GLFW_3)
	##END GLFW##

	##GLEW##
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLEW REQUIRED glew)
	##END GLEW##

	file(COPY src/shader.frag DESTINATION ${PROJECT_BINARY_DIR})
	file(COPY src/shader.vert DESTINATION ${PROJECT_BINARY_DIR})
	file(COPY src/shaderident.vert DESTINATION ${PROJECT_BINARY_DIR})

	file(COPY imgs/pieces DESTINATION ${PROJECT_BINARY_DIR}/imgs)
	file(COPY imgs DESTINATION ${PROJECT_BINARY_DIR})

endif(NOT DEFINED DUETTO)

add_subdirectory(srclib)
add_subdirectory(src)

