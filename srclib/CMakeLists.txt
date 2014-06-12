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
#
###BOX2D INCLUSION
set (BOX2D_INSTALL OFF)
set (BOX2D_INSTALL_DOC OFF)
IF(NOT DEFINED EMSCRIPTEN)
	set (BOX2D_BUILD_SHARED OFF)
	set (BOX2D_BUILD_STATIC ON)
ELSE(NOT DEFINED EMSCRIPTEN)
	set (BOX2D_BUILD_SHARED ON)
	set (BOX2D_BUILD_STATIC OFF)
ENDIF(NOT DEFINED EMSCRIPTEN)
set (BOX2D_BUILD_EXAMPLES OFF)

add_subdirectory (Box2D)
###END BOX2D INCLUSION

if(NOT DEFINED CHEERP)
	add_subdirectory(lodepng)
endif(NOT DEFINED CHEERP)
