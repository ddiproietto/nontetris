#ifndef _GLWRAPPER_H
#define _GLWRAPPER_H

#ifdef __MINGW32__
#define GLEW_STATIC
#endif

#define GLEW_NO_GLU
#include <GL/glew.h>

#endif
