Nontetris
=================

Nontetris is inspired by (i.e. a clone of) the awesome  [Not Tetris 2](http://stabyourself.net/nottetris2/) by Maurice Guégan.
It is a multiplatform demo (just playable) that uses GL for rendering. When I say multiplatform, I mean:

* **Web with cheerp**: it works at least with Firefox and Chrome, if WebGL is available. Play online [here](http://allievi.sssup.it/jacopone/cnontetris/)
* **Windows**: working with MXE cross compiler toolchain! Get the executable [here](https://allievi.sssup.it/jacopone/cnontetris-win/nontetris.zip)
* **Linux**: currently supported with glfw (You have to build the source yourself)
* **Web with emscripten**: it works at least with Firefox and Chrome, if WebGL is available. Play online [here](http://allievi.sssup.it/jacopone/cnontetris/index.html?compiler=emscripten)

Building
================

Get the source code with
	
	git clone https://github.com/ddiproietto/nontetris.git

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


Web with Cheerp
---------------

You just need [cheerp](http://leaningtech.com/cheerp/) and cmake.

	#compile
	mkdir cheerpbuild/
	cd cheerpbuild/
	cmake -DCMAKE_TOOLCHAIN_FILE=/opt/cheerp/share/cmake/Modules/CheerpToolchain.cmake ..
	make

See below for running instructions.

*Optional: you can minify all the JS in a single file with ./minimizecheerpjs.sh . You should edit the script to point to the google [closure compiler](https://developers.google.com/closure/compiler/), if you are using debian you can simply install the package libclosure-compiler-java. The script replaces the built JS with a minified one.*

Web with Emscripten
----------

Prerequisites: cmake(2.8.8), emscripten(tested with 1.10)

Edit the script emscriptenbuild.sh to reflect the Emscripten location in your system

	#Path to emscripten
	WHEREISEMSCRIPTEN=/usr/share/emscripten/

Emscripten provides glfw 2, so this option must be OFF in CMakeLists.txt

	option(USE_GLFW_3 "Use glfw3(otherwise use glfw2)" OFF)

Now by typing

	./emscriptenbuild.sh

the build process should start.

See below for running instructions.

Running the web version
-----------------------

Whether you used cheerp or emscripten (or both), the www/index.html is able to run the application. The www/ directory contains symlinks to the built JS files and to the resources necessary to run the application on the web.

Since XMLHttpRequest is used, you will need a static webserver serving the content from the www/ directory. The simplest way to setup one (on Linux) is to open a terminal, cd into the www/ directory and type

	python -m SimpleHTTPServer 8888

You can access localhost:8888 with your browser and play nontetris.

index.html embeds a JS snippet that chooses the scripts to load (nontetris-cheerp.js for cheerp, nontetris-emscripten.js and emscriptenwrapper.js for emscripten) based on the URL parameters. This happens on the client side and no server support is required. If one is interested only in one compiler, the scripts can be included directly.

Technology
==========

The code makes use of C++11 features (lambdas, std::array, std::chrono, initializer lists, ...)

This project uses the Box2D library. The code has been modified to be built successfully under Cheerp (no custom allocators, no unions). All the modification are enclosed by #ifdef __CHEERP__.

OpenGL is used to display graphics. The same codebase works on the native platform and on the web.

To load textures and external files (shaders), two small classes have been written (for Cheerp): src/texloader.h src/fileloader.h. They simply put the content on the DOM for a later retrieval and call a function when the resources have been loaded.

Directories
-----------

cheerpbuild/ cmake build directory for cheerp
emscriptenbuild/ cmake build directory for emscripten
build/ cmake build directory for the native target

imgs/ contains the textures in png format

src/ contains the sources of the project

srclib/ contains some libraries that have been embedded (Box2D and lodepng)

www/ contains html files and symlinks to the built code for cheerp and emscripten. After the build, it contains all the files necessary to run on the web.

Todo
====

(...) check TODO file.
Make it a game!

Acknowledgements
================

I would like to thank:

* The awesome people at [Leaningtech](http://leaningtech.com/): Alessandro, Massimo and Stefano
* A lot of friends who helped me with the math (and with some coding): Sbabbi, Peoro, Tom, Davide
* The man who made me discover the original Not Tetris 2: Enrico
