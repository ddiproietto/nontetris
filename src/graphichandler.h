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

#include "glwrapper.h"

#ifndef __DUETTO__

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

#include "gameoptions.h"

#include "texthandler.h"

struct GraphicPiece
{
	GLuint tex;
};

class GraphicHandler
{
	const GameOptions gameopt;

	#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	GLFWwindow * glfwwindow;
	#endif
	GLuint tex_background;
	GLuint tex_font;
	GLuint tex_small[7];

	GLuint piece_fbo;

	GLuint vbo_background;
	GLuint vbo_completeness;
	GLuint vbo_text;
	GLuint vbo_lines;
	GLuint vbo_piece;

	GLuint isp, compsp;

	GLint aGlobalVertexPositionLoc;
	GLint aGlobalTextureCoordLoc;
	GLint uGlobalPMatrixLoc;
	GLint uGlobalRTVecLoc;

	GLint aCompVertexPositionLoc;
	GLint aCompTextureCoordLoc;
	GLint uCompPMatrixLoc;
	GLint uCompRTVecLoc;
	GLint uCompLoc;
	GLint uCompColorLoc;

	GLfloat PMatrix_eye[16];
	GLfloat PMatrix_half[16];
	GLfloat PMatrix_pieces[16];
	GLfloat RTVec_eye[4];

public:
	GraphicHandler(const GameOptions & gameopt, const FileLoader & fileloader);
	~GraphicHandler();
	GraphicPiece * createpiece(const piece<float> & pol);
	void deletepiece(GraphicPiece * pgp);
	void beginrender(TextHandler & texthandler);
	void renderpiece(float x, float y, float rot, GraphicPiece * gp);
	void endrender(const std::vector<float> & linecompleteness, const std::vector<bool> & linecutblack);

	GraphicToInput toinput();
};

#endif //_GRAPHIC_HANDLER_H
