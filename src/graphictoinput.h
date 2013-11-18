#ifndef _GRAPHICTOINPUT
#define _GRAPHICTOINPUT

#ifndef __DUETTO__
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#define GLFW_NO_GLU
#include <GL/glfw.h>
#endif
#endif

struct GraphicToInput
{
#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	GLFWwindow * window;
#endif
};

#endif //_GRAPHICTOINPUT
