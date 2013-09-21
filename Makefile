#MAKEFILE for duetto: there should be a duettobuild directory, with cmake build already set up
all:
	make -C duettobuild/
	/opt/duetto/bin/llvm-link duettobuild/libnontetris.bc duettobuild/Box2D/Box2D/libBox2D.bc /opt/duetto/lib/libcxxabi.bc /opt/duetto/lib/libc.bc /opt/duetto/lib/libm.bc /opt/duetto/lib/libc++.bc -o duettobuild/output.bc
	-rm duettobuild/output.js
	/opt/duetto/bin/duetto-compiler duettobuild/output.bc
	mv duettobuild/output.js duettobuild/libnontetris.js

clean:
	make -C duettobuild/ clean

