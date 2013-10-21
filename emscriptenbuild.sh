#!/bin/sh
#Build script for emscripten: still not working
rm -Rf emscriptenbuild/
mkdir emscriptenbuild/

cd webbuild/
cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=~/homemade/emscripten/cmake/Platform/Emscripten_unix.cmake -DCMAKE_MODULE_PATH=~/homemade/emscripten/cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
~/homemade/emscripten/emmake make VERBOSE=1
mv nontetris nontetris.bc

cp ../shader.vert ../shader.frag ../shaderglobal.frag ../shaderglobal.vert ../imgs/pieces/{1,2,3,4,5,6,7}.png ../imgs/newgamebackground_pc.png .

~/homemade/emscripten/emcc -0x -O0 nontetris.bc Box2D/Box2D/libBox2D.so -o project.html --preload-file shader.vert --preload-file shader.frag --preload-file shaderglobal.vert --preload-file shaderglobal.frag --preload-file 1.png --preload-file 2.png --preload-file 3.png --preload-file 4.png --preload-file 5.png --preload-file 6.png --preload-file 7.png --preload-file newgamebackground_pc.png -s FORCE_GL_EMULATION=1
