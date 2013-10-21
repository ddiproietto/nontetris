#MAKEFILE for duetto: there should be a duettobuild directory, with cmake build already set up
all:
	make -C duettobuild/
	/opt/duetto/bin/llvm-link duettobuild/src/libnontetris.bc duettobuild/srclib/Box2D/Box2D/libBox2D.bc /opt/duetto/lib/libcxxabi.bc /opt/duetto/lib/libc.bc /opt/duetto/lib/libm.bc /opt/duetto/lib/libc++.bc -o duettobuild/output.bc
	-$(RM) duettobuild/output.js
	/opt/duetto/bin/duetto-compiler duettobuild/output.bc
	mv duettobuild/output.js duettobuild/libnontetris.js
	sed -i -e 's/print(/console.log(/' duettobuild/libnontetris.js

clean:
	make -C duettobuild/ clean

