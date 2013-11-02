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

configure_file (
	"NontetrisConfig.h.in"
	"${PROJECT_BINARY_DIR}/NontetrisConfig.h"
	)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/srclib/Box2D/")
if(NOT DEFINED DUETTO)
include_directories("${PROJECT_SOURCE_DIR}/srclib/lodepng/")
endif(NOT DEFINED DUETTO)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(MY_SOURCES nontetris.cpp physichandler.cpp inputhandler.cpp graphichandler.cpp inputhandler.cpp)

if(DEFINED DUETTO)
	set (MY_SOURCES ${MY_SOURCES} duettogl.cpp)
endif(DEFINED DUETTO)

if(DEFINED EMSCRIPTEN)
	set (MY_SOURCES ${MY_SOURCES} $<TARGET_OBJECTS:lodepng>)
endif(DEFINED EMSCRIPTEN)

add_executable (nontetris ${MY_SOURCES})

target_link_libraries(nontetris Box2D)

if(NOT DEFINED DUETTO)

	if(NOT DEFINED EMSCRIPTEN)
		target_link_libraries(nontetris ${OPENGL_LIBRARIES} lodepng)
		target_link_libraries(nontetris ${GLEW_LIBRARIES})
	endif(NOT DEFINED EMSCRIPTEN)

	if(USE_GLFW_3)
		target_link_libraries(nontetris ${GLFW_STATIC_LIBRARIES})#link statically
		#target_link_libraries(nontetris ${GLFW_LIBRARIES})#Link dynamically
	else(USE_GLFW_3)
		target_link_libraries(nontetris ${GLFW_LIBRARIES})
	endif(USE_GLFW_3)
endif(NOT DEFINED DUETTO)
