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
#include "graphichandler.h"

#include "NontetrisConfig.h"

#include "glwrapper.h"

#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#include <GLES2/webgles.h>
#else

#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#define GLFW_NO_GLU
#include <GL/glfw.h>
#endif

#endif

#include <algorithm>
#include <cmath>
#ifndef __DUETTO__
#include <iostream>
#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_CPP
#include "lodepng.h"
#endif
#include <string>
#include "fileloader.h"


#ifdef __DUETTO__
typedef GLsizei glvapt;
#else
typedef void * glvapt;
#endif

//FIND smallest power of two greater than width or height
int findsmallestpot(int x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return x + 1;
}
void printLog(GLuint obj, const std::string & str)
{
#ifndef __DUETTO__
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);

	if (infologLength > 0)
		std::cout << str <<infoLog << std::endl;
#endif
}

#define WEBPREAMBLE "precision mediump float;\n"

#if defined(EMSCRIPTEN) || defined(__MINGW32__) || defined(__DUETTO__)
#define DATAPATHPREAMBLE ""
#else
#define DATAPATHPREAMBLE "../"
#endif

GLuint filetoshader(const FileLoader & fl, GLenum shadertype, const std::string & filename)
{
	GLuint shader;

	std::string completefilename = std::string(DATAPATHPREAMBLE) + filename;
#ifdef __DUETTO__
	auto * c_ShaderSource = fl.getfilecontent(completefilename);
	if(shadertype == GL_FRAGMENT_SHADER)
			c_ShaderSource = client::String(WEBPREAMBLE).concat(c_ShaderSource);
#else
	std::string ShaderSource = FileLoader::getfilecontent(completefilename);
#ifdef EMSCRIPTEN
	if(shadertype == GL_FRAGMENT_SHADER)
		ShaderSource = std::string(WEBPREAMBLE) + ShaderSource;
#endif
	const char * c_ShaderSource = ShaderSource.c_str();
#endif

	shader = glCreateShader(shadertype);
#ifdef __DUETTO__
	webGLES->shaderSource(webGLESLookupWebGLShader(shader), *c_ShaderSource);
#else
	glShaderSource(shader, 1, &c_ShaderSource, NULL);
#endif
	glCompileShader(shader);
	printLog(shader, filename);

	return shader;
}

void settexparameter(GLenum pname, GLfloat param)
{
	glTexParameterf (GL_TEXTURE_2D, pname, param);
}
void settexparameter(GLenum pname, GLint param)
{
	glTexParameteri (GL_TEXTURE_2D, pname, param);
}

template<typename... Args>
void settexparameters()
{

}

template<typename T, typename... Args>
void settexparameters(GLenum pname, T param, Args... texparameters)
{
	settexparameter (pname, param);
	settexparameters (texparameters...);
}

template<typename... Args>
GLuint filetotexture(const std::string & filename, bool generatemipmap, Args... texparameters)
{
	GLuint rettex;
	glGenTextures(1, &rettex);
	glBindTexture( GL_TEXTURE_2D, rettex);
	unsigned int twidth, theight;

#ifndef __DUETTO__
	unsigned char * image;
	lodepng_decode24_file(&image, &twidth, &theight, filename.c_str());

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image );
	free(image);
#else
	webGLES->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById(filename.c_str())));
#endif
	settexparameters(texparameters...);

	if (generatemipmap)
		glGenerateMipmap (GL_TEXTURE_2D);
	return rettex;
}

GraphicHandler::GraphicHandler(const GameOptions & _gameopt, const FileLoader & fileloader):gameopt(_gameopt), vbo_text(0), PMatrix_eye{0.0F}, PMatrix_half{0.0F}, PMatrix_pieces{0.0F}, RTVec_eye{0.0F}
{
	// WINDOWING INITIALIZATION
#ifndef __DUETTO__
	glfwInit();
#if GLFW_VERSION_MAJOR == 3
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwwindow = glfwCreateWindow(gameopt.width, gameopt.height, "nontetris", gameopt.fullscreen?glfwGetPrimaryMonitor():NULL, NULL);
	glfwMakeContextCurrent(glfwwindow);
#else
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
	glfwOpenWindow(gameopt.width, gameopt.height, 5, 6, 5, 8, 0, 0, gameopt.fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );
	glfwSetWindowTitle("nontetris");
#endif
#ifndef EMSCRIPTEN
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK)
		std::cerr<<"GLEW fail"<<std::endl;
	glEnableClientState(GL_VERTEX_ARRAY);
#endif //defined(__EMSCRIPTEN__)
#else //defined(__DUETTO__)
	webGLESInit("glcanvas");
#endif

	// GL FEATURES
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	// SHADER COMPILATION
	GLuint vs     = filetoshader(fileloader, GL_VERTEX_SHADER, "shader.vert"), /* Vertex Shader */
	       fs     = filetoshader(fileloader, GL_FRAGMENT_SHADER, "shader.frag"); /* Fragment Shader with textures */

	isp = glCreateProgram();
	glAttachShader(isp, vs);
	glAttachShader(isp, fs);
	glLinkProgram(isp);
	printLog(isp, "linking global shader:");

	glUseProgram(isp);
	aGlobalVertexPositionLoc = glGetAttribLocation(isp, "aVertexPosition");
	aGlobalTextureCoordLoc = glGetAttribLocation(isp, "aTextureCoord");
	uGlobalPMatrixLoc = glGetUniformLocation(isp, "uPMatrix");
	uGlobalRTVecLoc = glGetUniformLocation(isp, "uRTVec");


	if (gameopt.gametype == GameOptions::CUTTING)
	{
		GLuint compfs = filetoshader(fileloader, GL_FRAGMENT_SHADER, "shadercomp.frag"); /* Fragment Shader for bars (no textures) */
		compsp = glCreateProgram();
		glAttachShader(compsp, vs);
		glAttachShader(compsp, compfs);
		glLinkProgram(compsp);
		printLog(compsp, "linking completeness shader:");

		glUseProgram(compsp);
		aCompVertexPositionLoc = glGetAttribLocation(compsp, "aVertexPosition");
		aCompTextureCoordLoc = glGetAttribLocation(compsp, "aTextureCoord");
		uCompLoc = glGetUniformLocation(compsp, "uComp");
		uCompColorLoc = glGetUniformLocation(compsp, "uColor");
		uCompPMatrixLoc = glGetUniformLocation(compsp, "uPMatrix");
		uCompRTVecLoc = glGetUniformLocation(compsp, "uRTVec");
	}

	// TEXTURES LOADING
	const char * backgroundfilename = NULL;
	if (gameopt.gametype == GameOptions::CUTTING)
		backgroundfilename = DATAPATHPREAMBLE "imgs/newgamebackgroundgamea.png";
	else if (gameopt.gametype == GameOptions::STACK)
		backgroundfilename = DATAPATHPREAMBLE "imgs/newgamebackground.png";

	tex_background = filetotexture(backgroundfilename, true, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (int i = 0; i < 7; i++)
	{
		std::string path = std::string(DATAPATHPREAMBLE "imgs/pieces/")+std::to_string(i+1)+std::string(".png");

		tex_small[i] = filetotexture(path, false, GL_TEXTURE_MIN_FILTER, GL_LINEAR, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	tex_font = filetotexture(DATAPATHPREAMBLE "imgs/font.png", true, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// VBO INITIALIZATION
	glGenBuffers(1, &vbo_background);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_background);
	float rapx = 256.0/160.0;
	float rapy = 256.0/144.0;
	GLfloat vertices2 [] = {-1, -1,  0, 1/rapy,
				-1,  1,  0, 0,
				 1, -1,  1/rapx, 1/rapy,
				 1,  1,  1/rapx, 0};
	glBufferData(GL_ARRAY_BUFFER, 4*2*2*sizeof(float), vertices2, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_piece);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_piece);
	std::vector<GLfloat> vertices_piece;
	vertices_piece.insert(vertices_piece.end(),
		{ -2.0F, -2.0F, 0, 0,
		  -2.0F,  2.0F, 0, 1,
		   2.0F, -2.0F, 1, 0,
		   2.0F,  2.0F, 1, 1});
	glBufferData(GL_ARRAY_BUFFER, vertices_piece.size()*sizeof(float), vertices_piece.data(), GL_STATIC_DRAW);

	if (gameopt.gametype == GameOptions::CUTTING)
	{
		glGenBuffers(1, &vbo_completeness);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_completeness);
		std::vector<GLfloat> vertices_side;
		for(float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
		{
			vertices_side.insert(vertices_side.end(),
				{-1,                1.0F - i * (2.0F/gameopt.rows),             0, 1,
				 -1,                1.0F - (i+gameopt.rowwidth) *(2.0F/gameopt.rows),   0, 0,
				 -1 + 6*(2.0F/160), 1.0F - i * (2.0F/gameopt.rows),             1, 1,
				 -1 + 6*(2.0F/160), 1.0F - (i+gameopt.rowwidth) *(2.0F/gameopt.rows),   1, 0,  });
		}
		glBufferData(GL_ARRAY_BUFFER, vertices_side.size()*sizeof(float), vertices_side.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &vbo_lines);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
		std::vector<GLfloat> vertices_lines;
		for(float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
		{
			vertices_lines.insert(vertices_lines.end(),
				{-1 + 14*(2.0F/160), 1.0F - i * (2.0F/gameopt.rows),             0, 1,
				 -1 + 14*(2.0F/160), 1.0F - (i+gameopt.rowwidth) *(2.0F/gameopt.rows),   0, 0,
				 -1 + 96*(2.0F/160), 1.0F - i * (2.0F/gameopt.rows),             1, 1,
				 -1 + 96*(2.0F/160), 1.0F - (i+gameopt.rowwidth) *(2.0F/gameopt.rows),   1, 0,  });
		}
		glBufferData(GL_ARRAY_BUFFER, vertices_lines.size()*sizeof(float), vertices_lines.data(), GL_STATIC_DRAW);
	}

	// PROJECTION MATRICES INITIALIZATION
	RTVec_eye[1] = 1.0F;

	float left = -1.75F, right = 18.25F, top = 0.0F, bottom = 18.0F, far = -1.0F, near = 1.0F;
	if (gameopt.gametype == GameOptions::CUTTING)
		left = -1.75F, right = 18.25F;
	else if (gameopt.gametype == GameOptions::STACK)
		left = -2.0F, right = 18.0F;
	PMatrix_pieces[0] = -2 / (left - right);
	PMatrix_pieces[5] = -2 / (bottom - top);
	PMatrix_pieces[10] = 2 / (near - far);
	PMatrix_pieces[12] = (left + right) / (left - right);
	PMatrix_pieces[13] = (bottom + top) / (bottom - top);
	PMatrix_pieces[14] = (near + far) / (near - far);
	PMatrix_pieces[15] = 1;

	PMatrix_eye[0] = PMatrix_eye[5] = PMatrix_eye[10] = PMatrix_eye[15] = 1.0F;

	PMatrix_half[0] = PMatrix_half[5] = PMatrix_half[10] = 0.5F;
	PMatrix_half[15] = 1.0F;

	if (gameopt.gametype == GameOptions::CUTTING)
	{
		glUseProgram(compsp);

		glUniformMatrix4fv(uCompPMatrixLoc, 1, false, PMatrix_eye);
		glUniform4fv(uCompRTVecLoc, 1, RTVec_eye);
	}

	// PIECE FBO INITIALIZATION
	glGenFramebuffers(1, &piece_fbo);
}

GraphicHandler::~GraphicHandler()
{
#ifndef __DUETTO__
#if GLFW_VERSION_MAJOR == 3
	glfwDestroyWindow(glfwwindow);
#else
	glfwCloseWindow();
#endif
	glfwTerminate();
#endif
}

GraphicToInput GraphicHandler::toinput()
{
#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	return GraphicToInput{.window=glfwwindow};
#else
	return GraphicToInput();
#endif
}

// Very simple tessellation: FROM counterclockwise list of vertices TO triangle strip
// Since all polygons are convex all we need to do is reorder the vertices.
// e.g. 1 2 3 4 5 6 -> 1 2 6 3 4 5
// If the piece is made of two convex polygons we simply put twice the first vertex of the second polygon,
// thus creating a degenerate triangle. This works properly if:
// - the first vertex of the second polygon lies on a side of the first polygon.
GraphicPiece * GraphicHandler::createpiece(const piece<float> & pie)
{
	GraphicPiece * pgp = new GraphicPiece;
	GLuint VBOid;
	size_t size = (pie.totsize()+(pie.size()-1))*4;
	std::vector<GLfloat> vertices;
	vertices.reserve(size);
	bool firstshape = true;
	for (const auto & pol: pie )
	{
		if ( firstshape )
		{
			firstshape = false;
		}
		else
		{
			vertices.push_back(pol[0].x);
			vertices.push_back(pol[0].y);
			vertices.push_back((pol[0].x-2.0)/4);
			vertices.push_back((pol[0].y-2.0)/4);
		}
		vertices.push_back(pol[0].x);
		vertices.push_back(pol[0].y);
		vertices.push_back((pol[0].x-2.0)/4);
		vertices.push_back((pol[0].y-2.0)/4);
		//std::cerr<<0<<std::endl;

		for ( unsigned int i = 1; i <= pol.size()/2; ++i)
		{
			auto & vert = pol[i];

			vertices.push_back(vert.x);
			vertices.push_back(vert.y);
			vertices.push_back((vert.x-2.0)/4);
			vertices.push_back((vert.y-2.0)/4);
			//std::cerr<<i<<std::endl;
			if(i < (pol.size()+1)/2)
			{
				auto & vert = pol[pol.size()-i];
				//std::cerr<<pol.size()-i<<std::endl;
				vertices.push_back(vert.x);
				vertices.push_back(vert.y);
				vertices.push_back((vert.x-2.0)/4);
				vertices.push_back((vert.y-2.0)/4);
			}
		}
	}

	glUseProgram(isp);
	glUniform4fv(uGlobalRTVecLoc, 1, RTVec_eye);
	glUniformMatrix4fv(uGlobalPMatrixLoc, 1, false, PMatrix_half);

	glGenBuffers(1, &VBOid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOid);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint piecetex;

	glEnableVertexAttribArray(aGlobalVertexPositionLoc);
	glEnableVertexAttribArray(aGlobalTextureCoordLoc);
	glGenTextures(1, &piecetex);
	glBindTexture(GL_TEXTURE_2D, piecetex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float imgquad = gameopt.height/gameopt.rows;
	int piecefbosize = findsmallestpot(gameopt.piecesAA*4*imgquad);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, piecefbosize, piecefbosize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, piece_fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, piecetex, 0);
	glViewport(0, 0, piecefbosize, piecefbosize);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear( GL_COLOR_BUFFER_BIT );

	glBindTexture(GL_TEXTURE_2D, tex_small[pie.getType()]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOid);

	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()/4);

	glBindTexture(GL_TEXTURE_2D, piecetex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisableVertexAttribArray(aGlobalVertexPositionLoc);
	glDisableVertexAttribArray(aGlobalTextureCoordLoc);

	glDeleteBuffers(1, &VBOid);
	pgp->tex = piecetex;
	//std::cerr<<"NUM:"<<gp.num<<std::endl;
	return pgp;
}

void GraphicHandler::deletepiece(GraphicPiece * pgp)
{
	glDeleteTextures(1,&(pgp->tex));
	delete pgp;
}

void GraphicHandler::beginrender(TextHandler & texthandler)
{
	glViewport(0, 0, gameopt.width, gameopt.height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear( GL_COLOR_BUFFER_BIT );

	//DRAW BACKGROUND
	glUseProgram(isp);
	glUniform4fv(uGlobalRTVecLoc, 1, RTVec_eye);
	glUniformMatrix4fv(uGlobalPMatrixLoc, 1, false, PMatrix_eye);

	glEnableVertexAttribArray(aGlobalVertexPositionLoc);
	glEnableVertexAttribArray(aGlobalTextureCoordLoc);

	// BACKGROUND IMAGE
	glBindBuffer(GL_ARRAY_BUFFER, vbo_background);
	glBindTexture(GL_TEXTURE_2D, tex_background);
	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// TEXT
	glBindBuffer(GL_ARRAY_BUFFER, texthandler.getvbo());
	glBindTexture(GL_TEXTURE_2D, tex_font);
	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	int sum = 0;
	for (int i: texthandler.getvbo_numvertices())
	{
		glDrawArrays(GL_TRIANGLE_STRIP, sum, i);
		sum += i;
	}

	//END DRAW BACKGROUND
	glUniformMatrix4fv(uGlobalPMatrixLoc, 1, false, PMatrix_pieces);
}


void GraphicHandler::renderpiece(float x, float y, float rot, GraphicPiece * gp)
{
	GLfloat RTVec[4] = {(GLfloat)sin(rot), (GLfloat)cos(rot), x, y};
	glUniform4fv(uGlobalRTVecLoc, 1, RTVec);

	glBindTexture(GL_TEXTURE_2D, gp->tex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_piece);

	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GraphicHandler::endrender(const std::vector<float> & linecompleteness, const std::vector<bool> & linecutblack)
{
	glDisableVertexAttribArray(aGlobalVertexPositionLoc);
	glDisableVertexAttribArray(aGlobalTextureCoordLoc);

	if (gameopt.gametype== GameOptions::CUTTING)
	{
		// LINE COMPLETENESS

		glUseProgram(compsp);

		glEnableVertexAttribArray(aCompVertexPositionLoc);
		glEnableVertexAttribArray(aCompTextureCoordLoc);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_completeness);
		glVertexAttribPointer(aCompVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
		glVertexAttribPointer(aCompTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
		int lineind = 0;
		for(float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth, ++lineind)
		{
			glUniform1f(uCompLoc, linecompleteness[i]);
			float colbase;
			colbase = 1.0 - (0.8*linecompleteness[i]);
			if (linecompleteness[i]>=1.0)
				colbase -= 0.2;
			glUniform3f(uCompColorLoc, colbase, colbase, colbase);
			glDrawArrays(GL_TRIANGLE_STRIP, lineind*4, 4);
		}

		// LINE CUT BLACK
		glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
		glVertexAttribPointer(aCompVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
		glVertexAttribPointer(aCompTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
		glUniform3f(uCompColorLoc, 205.0F/255.0F, 175.0F/255.0F, 145.0F/255.0F);
		glUniform1f(uCompLoc, 1.0);
		lineind = 0;
		for(float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth, ++lineind)
		{
			if(linecutblack[i])
			{
				glDrawArrays(GL_TRIANGLE_STRIP, lineind*4, 4);
			}
		}

		glDisableVertexAttribArray(aCompVertexPositionLoc);
		glDisableVertexAttribArray(aCompTextureCoordLoc);
	}

#ifndef __DUETTO__
#if GLFW_VERSION_MAJOR == 3
	glfwSwapBuffers(glfwwindow);
	glfwPollEvents();
#else
	glfwSwapBuffers();
#endif
#endif
}
