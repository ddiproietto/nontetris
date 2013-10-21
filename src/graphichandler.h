#ifndef _GRAPHIC_HANDLER_H
#define _GRAPHIC_HANDLER_H

#include "NontetrisConfig.h"

#ifdef __DUETTO__
#include "duettogl.h"
#else

#define GLEW_NO_GLU
#include <GL/glew.h>

#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif

#endif

#include "polygon.h"
#include "piece.h"
#include "fileloader.h"

#include <functional>
#include "NontetrisConfig.h"


struct GraphicPiece
{
	GLuint VBOid;
	GLuint num;
	int tex;
};

class GraphicHandler
{
	#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	GLFWwindow * glfwwindow;
	#endif
	GLint uPMatrixLoc;
	GLint uRTVecLoc;
	GLint aVertexPositionLoc;
	GLuint tex_background;
	GLuint tex[7];

	GLuint pieces_fbo[7];
	GLuint vbo_background;

	int width;
	int height;

	GLuint sp, isp;
	GLint aGlobalVertexPositionLoc;
	GLint aGlobalTextureCoordLoc;

public:
	GraphicHandler(int width = 600, int height = 540, bool fullscreen = false, FileLoader * fileloader = NULL);
	~GraphicHandler();
	GraphicPiece * createpiece(piece<float> pol);
	bool render(const std::function< void(const std::function<void(float x, float y, float rot, GraphicPiece * d)> &)> & allbodies );

	#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	GLFWwindow * getglfwwindow();
	#endif
};

#endif //_GRAPHIC_HANDLER_H
