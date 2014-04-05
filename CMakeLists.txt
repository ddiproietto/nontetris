#*****************************************************************************
#
#	 Copyright (C) 2013  Daniele Di Proietto <d.diproietto@sssup.it>
#	 
#	 This file is part of nontetris.
#	 
#	 nontetris is free software: you can redistribute it and/or modify
#	 it under the terms of the GNU General Public License as published by
#	 the Free Software Foundation, either version 3 of the License, or
#	 (at your option) any later version.
#	 
#	 nontetris is distributed in the hope that it will be useful,
#	 but WITHOUT ANY WARRANTY; without even the implied warranty of
#	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	 GNU General Public License for more details.
#	 
#	 You should have received a copy of the GNU General Public License
#	 along with nontetris.  If not, see <http://www.gnu.org/licenses/>.
#
#*****************************************************************************
if(DEFINED DUETTO)
	cmake_minimum_required (VERSION 2.6) #Decent version
else(DEFINED DUETTO)
	if(NOT DEFINED EMSCRIPTEN)
		cmake_minimum_required (VERSION 2.8) #For file( COPY)
	else(NOT DEFINED EMSCRIPTEN)
		cmake_minimum_required (VERSION 2.8.8) #For object libraries
	endif(NOT DEFINED EMSCRIPTEN)
endif(DEFINED DUETTO)
project (Nontetris)

set (NONTETRIS_VERSION_MAJOR 0)
set (NONTETRIS_VERSION_MINOR 1)

option(USE_GLFW_3 "Use glfw3(otherwise use glfw2)" OFF)

if(USE_GLFW_3)
set(USE_GLFW_VERSION 3)
else(USE_GLFW_3)
set(USE_GLFW_VERSION 2)
endif(USE_GLFW_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

IF(DEFINED DUETTO)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
ENDIF(DEFINED DUETTO)

IF(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
ENDIF(MINGW)

if(NOT DEFINED DUETTO)
	if(NOT DEFINED EMSCRIPTEN)
		##OpenGL##
		find_package(OpenGL REQUIRED)
		##END OpenGL##

		find_package(PkgConfig REQUIRED)
		##GLFW##
		if(USE_GLFW_3)
			pkg_search_module(GLFW REQUIRED glfw3)
		else(USE_GLFW_3)
			pkg_search_module(GLFW REQUIRED libglfw)
		endif(USE_GLFW_3)
		##END GLFW##

		##GLEW##
		pkg_search_module(GLEW REQUIRED glew)
		##END GLEW##
	endif(NOT DEFINED EMSCRIPTEN)

	file(COPY src/shader.frag DESTINATION ${PROJECT_BINARY_DIR})
	file(COPY src/shader.vert DESTINATION ${PROJECT_BINARY_DIR})
	file(COPY src/shadercomp.frag DESTINATION ${PROJECT_BINARY_DIR})

	file(COPY imgs/pieces DESTINATION ${PROJECT_BINARY_DIR}/imgs)
	file(COPY imgs DESTINATION ${PROJECT_BINARY_DIR})

endif(NOT DEFINED DUETTO)

add_subdirectory(srclib)
add_subdirectory(src)
