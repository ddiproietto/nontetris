#!/bin/sh
#Build script for emscripten: still not working

#Path to emscripten
WHEREISEMSCRIPTEN=~/homemade/emscripten/

rm -Rf emscriptenbuild/
mkdir emscriptenbuild/

cd emscriptenbuild/
cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE="$WHEREISEMSCRIPTEN"/cmake/Platform/Emscripten_unix.cmake -DCMAKE_MODULE_PATH="$WHEREISEMSCRIPTEN"/cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
"$WHEREISEMSCRIPTEN"/emmake make VERBOSE=1
mv src/nontetris nontetris.bc

#cp ../src/shader.vert ../src/shader.frag ../src/shaderident.vert ../imgs/pieces/*.png ../imgs/newgamebackground.png .
#srclib/lodepng/liblodepng.so 
"$WHEREISEMSCRIPTEN"/emcc -0x -O0 nontetris.bc srclib/Box2D/Box2D/libBox2D.so -o project.html --preload-file shader.vert --preload-file shader.frag --preload-file shaderident.vert --preload-file imgs/pieces/1.png --preload-file imgs/pieces/2.png --preload-file imgs/pieces/3.png --preload-file imgs/pieces/4.png --preload-file imgs/pieces/5.png --preload-file imgs/pieces/6.png --preload-file imgs/pieces/7.png --preload-file imgs/newgamebackground.png -s FORCE_GL_EMULATION=1
