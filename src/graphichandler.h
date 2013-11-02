/*****************************************************************************

     Copyright (C) 2013  Daniele Di Proietto <d.diproietto@sssup.it>
     
     This file is part of nontetris.
     
     nontetris is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.
     
     nontetris is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with nontetris.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/
#ifndef _GRAPHIC_HANDLER_H
#define _GRAPHIC_HANDLER_H

#include "NontetrisConfig.h"

#ifdef __DUETTO__
#include "duettogl.h"
#else

#include "glwrapper.h"
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#define GLFW_NO_GLU
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
