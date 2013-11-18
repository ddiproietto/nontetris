Nontetris
=================

Nontetris is inspired by (i.e. a clone of) the awesome  [Not Tetris 2](http://stabyourself.net/nottetris2/) by Maurice Guégan.
It is a multiplatform demo (just playable) that uses GL for rendering. When I say multiplatform, I mean:

* **Web with duetto**: it works at least with Firefox and Chrome, if WebGL is available. It still has issues, but we hope to fix them soon. Play online [here](http://allievi.sssup.it/jacopone/cnontetris/)
* **Windows**: working with MXE cross compiler toolchain! Get the executable [here](https://allievi.sssup.it/jacopone/cnontetris-win/nontetris.zip)
* **Linux**: currently supported with glfw (You have to build the source yourself)
* **Web with emscripten**: work in progress

Building
================

Get the source code with
	
	git clone git@github.com:ddiproietto/nontetris.git

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

If you manage to install GLEW and GLFW in your favourite compiler (Visual Studio 2012, maybe?) you're free to try.
Here I'm describing the procedure to cross compile from linux using [MXE](http://mxe.cc/).

* Download [MXE](http://mxe.cc/#download). Now you can build libraries as explained [here](http://mxe.cc/#usage)
* Build at least gcc, glew, glfw2 (You can also use glfw3, if you want. You'll have to change the CMakeList.txt, as explained for linux)
	
		make gcc glew glfw2

* Change directory to nontetris sources and then
	
		mkdir windowsbuild/
		cd windowsbuild/
		cmake -DCMAKE_TOOLCHAIN_FILE=/PATH_TO_MXE/usr/i686-pc-mingw32/share/cmake/mxe-conf.cmake ..
		make


* Now you will find in windowsbuild/src/ an executable named nontetris.exe. Yay! Rembember to run it in the same folder as shader.vert, shader.frag, shaderident.vert and the imgs/ folder


Web with Duetto
---------------

You just need [duetto](http://leaningtech.com/duetto/) and cmake.

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

*Optional: you can minify all the JS in a single file with ./minimizejs.sh . You should edit the script to point to the google [closure compiler](https://developers.google.com/closure/compiler/). The script puts the minified JS in www/nontetris.js.min. You may now edit www/index.html to include only the minified script*

Emscripten
----------

Prerequisites: cmake(2.8.8)

Edit the script emscriptenbuild.sh to reflect the Emscripten location in your system

	#Path to emscripten
	WHEREISEMSCRIPTEN=~/homemade/emscripten/

Now by typing

	./emscriptenbuild.sh

the build process should start. The built file is emscriptenbuild/project.html.
Unfortunately there is a problem with Emscripten GL wrapper, which I hope to investigate.

Technology
==========

The code makes use of C++11 features (lambdas, std::array, std::chrono, initializer lists, ...)

This project uses the Box2D library. The code has been modified to be built successfully under Duetto(no custom allocators, no unions). All the modification are enclosed by #ifdef __DUETTO__.

OpenGL is used to display graphics. The same codebase works on the native platform and on the web: a small wrapper has been written for duetto(src/duettogl.cpp) to map some basic GL calls to WebGL.

To load textures and external files(shaders) two small classes have been written (for duetto): src/texloader.h src/fileloader.h. They simply put the content on the DOM for a later retrieval and call a function when the resources have been loaded.

Directories
-----------

duettobuild/ cmake build directory for duetto
emscriptenbuild/ cmake build directory for emscripten
build/ cmake build directory for the native target

imgs/ contains the textures in png format

src/ contains the sources of the project

srclib/ contains some libraries that have been embedded (Box2D and lodepng)

www/ contains html files and symlink to the built code for duetto. After the build it contains all the files necessary to run on the web.

Todo
====

Fix the build under emscripten.
Fix Box2D glitches with duetto.
(...) check TODO file.
Make it a game!

Acknowledgements
================

I would like to thank:

* The awesome people at [Leaningtech](http://leaningtech.com/): Alessandro, Massimo and Stefano
* A lot of friends who helped me with the math (and with some coding): Sbabbi, Peoro, Tom, Davide
* The man who made me discover the original Not Tetris 2: Enrico
