Nontetris
=================

Nontetris is a multiplatform demo that uses GL for rendering. When I say multiplatform, I mean:

* Linux: currently supported with glfw
* Windows: it used to work with mingw, the new code has still to be tested
* Web with duetto: it works at least with Firefox and Chrome, if WebGL is available
* Web with emscripten: work in progress

Building
================

Linux
---------------

Prerequisites: cmake(2.8), glfw(2 or 3), glew, GL headers(mesa-dev).

Edit CMakeLists.txt and change the line

	option(USE_GLFW_3 "Use glfw3(otherwise use glfw2)" OFF)

to match the version of GLFW you want to build with.

If you are using GLFW3, you could edit src/CMakeLists.txt to choose whether or not link statically with glfw3

	target_link_libraries(nontetris ${GLFW_STATIC_LIBRARIES})#link statically
	#target_link_libraries(nontetris ${GLFW_LIBRARIES})#Link dynamically

The run the following commands to build and run

	#compile
	mkdir build/
	cd build
	cmake ..
	make
	#run
	cd src/
	./nontetris

Windows
-------

I've successully managed once to do a build with mingw-w64. You need a pretty recent version(g++ 4.7 should be fine), since the code is using C++11.

Web with Duetto
---------------

You just need duetto(http://leaningtech.com/duetto/) and cmake.

	#compile
	mkdir duettobuild/
	cd duettobuild/
	cmake -DCMAKE_TOOLCHAIN_FILE=/opt/duetto/share/cmake/Modules/DuettoToolchain.cmake ..
	make

After the build is finished you will find some useful symlinks to the build JS files and JS duetto libraries in www/ directory. Now, if you start a webserver, serving static content from that folder

	cd ..
	cd www/
	python -m SimpleHTTPServer 8888

You can access localhost:8888 with your browser and play nontetris.

*Optional: you can minify all the JS in a single file with ./minimizejs.sh . You should edit the script to point to the google closure compiler(https://developers.google.com/closure/compiler/). The script puts the minified JS in www/nontetris.js.min. You may now edit www/index.html to include only the minified script*

Emscripten
----------

Edit the script emscriptenbuild.sh to reflect the Emscripten location in your system

	#Path to emscripten
	WHEREISEMSCRIPTEN=~/homemade/emscripten/

Now by typing

	./emscriptenbuild.sh

the build process should start. The built file is emscriptenbuild/project.html.
Unfortunately there is a problem with Emscripten GL wrapper, which I hope to investigate.

Technology
==========

The code makes use of C++11 features (lambdas, std::array, initializer lists)

This project uses the Box2D library. The code has been modified to be built successfully under Duetto(no custom allocators, no unions). All the modification are enclosed by #ifdef __DUETTO__.

OpenGL is used to display graphics. The same codebase works on the native platform and on the web: a small wrapper has been written for duetto(src/duettogl.cpp) to map some basic GL calls to WebGL.

To load textures and external files(shaders) two small classes have been written (for duetto): src/texloader.h src/fileloader.h. They simply put the content on the DOM for a later retrieval and call a function when the resources have been loaded.

Directories
-----------

duettobuild/ cmake build directory for duetto
emscriptenbuild/ cmake build directory for emscripten
build/ cmake build directory for the native target

imgs/ contains the textures in png format

src/ contains the sources of the demo

srclib/ contains some libraries that have been embedded (Box2D and lodepng)

www/ contains html files and symlink to the built code for duetto. After the build it contains all the files necessary to run on the web.

