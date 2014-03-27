#!/bin/sh
#
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
#Build script for emscripten

#Path to emscripten
WHEREISEMSCRIPTEN=/usr/share/emscripten/

if [ ! -d "emscriptenbuild" ]; then
	rm -f emscriptenbuild
	mkdir emscriptenbuild/
fi

cd emscriptenbuild/
cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE="$WHEREISEMSCRIPTEN"/cmake/Platform/Emscripten.cmake -DCMAKE_MODULE_PATH="$WHEREISEMSCRIPTEN"/cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
"$WHEREISEMSCRIPTEN"/emmake make
mv src/nontetris nontetris.bc

"$WHEREISEMSCRIPTEN"/emcc -O3 nontetris.bc srclib/Box2D/Box2D/libBox2D.so -o nontetris-emscripten.js --preload-file shader.vert --preload-file shader.frag --preload-file shaderident.vert --preload-file imgs/pieces/1.png --preload-file imgs/pieces/2.png --preload-file imgs/pieces/3.png --preload-file imgs/pieces/4.png --preload-file imgs/pieces/5.png --preload-file imgs/pieces/6.png --preload-file imgs/pieces/7.png --preload-file imgs/newgamebackground.png --preload-file imgs/font.png
