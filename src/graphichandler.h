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
#include <GLES2/gl2.h>
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
#include <array>
#include <vector>

#include "graphictoinput.h"

struct GraphicOptions
{
	int width;
	int height;
	bool fullscreen;
};

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
	GLuint tex_background;
	GLuint tex_font;
	GLuint tex[7];

	GLuint pieces_fbo[7];
	GLuint vbo_background;
	GLuint vbo_completeness;
	GLuint vbo_score;
	std::array<int, 3> vbo_score_num_vertices;
	GLuint vbo_lines;

	int width;
	int height;
	float rows;
	float rowwidth;

	GLuint sp, isp, compsp;
	GLint aGlobalVertexPositionLoc;
	GLint aGlobalTextureCoordLoc;
	GLint uPMatrixLoc;
	GLint uRTVecLoc;
	GLint aVertexPositionLoc;
	GLint aCompVertexPositionLoc;
	GLint aCompTextureCoordLoc;
	GLint uCompLoc;

public:
	GraphicHandler(const GraphicOptions & gopt, const FileLoader & fileloader, float _rows, float _rowwidth);
	~GraphicHandler();
	GraphicPiece * createpiece(piece<float> pol);
	void deletepiece(GraphicPiece * pgp);
	void beginrender();
	void renderpiece(float x, float y, float rot, GraphicPiece * gp);
	void endrender(const std::vector<float> & linecompleteness, const std::vector<bool> & linecutblack);

	void updatescore(int number_a, int number_b, int number_c);

	GraphicToInput toinput();
};

#endif //_GRAPHIC_HANDLER_H
