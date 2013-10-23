if(NOT DEFINED EMSCRIPTEN)
	add_library(lodepng STATIC lodepng.cpp)
else(NOT DEFINED EMSCRIPTEN)
	#Emscripten doesn't support well static libraries
	add_library(lodepng OBJECT lodepng.cpp)
endif(NOT DEFINED EMSCRIPTEN)

SET_TARGET_PROPERTIES(lodepng PROPERTIES COMPILE_FLAGS "-DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_CPP")
