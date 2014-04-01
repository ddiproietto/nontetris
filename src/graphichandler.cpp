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

#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#include <GLES2/gl2.h>
#include <GLES2/webgles.h>
#else

#include "glwrapper.h"

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
void ortho(GLfloat * out, double left, double right, double bottom, double top, double near, double far)
{
	//COPIED FROM javascript gl-matrix
	double lr = 1 / (left - right),
	       bt = 1 / (bottom - top),
	       nf = 1 / (near - far);
	out[0] = -2 * lr;
	out[1] = 0;
	out[2] = 0;
	out[3] = 0;
	out[4] = 0;
	out[5] = -2 * bt;
	out[6] = 0;
	out[7] = 0;
	out[8] = 0;
	out[9] = 0;
	out[10] = 2 * nf;
	out[11] = 0;
	out[12] = (left + right) * lr;
	out[13] = (top + bottom) * bt;
	out[14] = (far + near) * nf;
	out[15] = 1;
}

void eye(GLfloat * out)
{
	out[0] = 1;
	out[1] = 0;
	out[2] = 0;
	out[3] = 0;
	out[4] = 0;
	out[5] = 1;
	out[6] = 0;
	out[7] = 0;
	out[8] = 0;
	out[9] = 0;
	out[10] = 1;
	out[11] = 0;
	out[12] = 0;
	out[13] = 0;
	out[14] = 0;
	out[15] = 1;
}

#define WEBPREAMBLE "precision mediump float;\n"

#if defined(EMSCRIPTEN) || defined(__MINGW32__)
#define DATAPATHPREAMBLE ""
#else
#define DATAPATHPREAMBLE "../"
#endif


GraphicHandler::GraphicHandler(const GraphicOptions & gopt, const FileLoader & fileloader, float _rows, float _rowwidth):width(gopt.width), height(gopt.height), vbo_score(0),rows(_rows), rowwidth(_rowwidth)
{
	float imgquad = height/rows;
	float piecesAA = 2;

	#ifndef __DUETTO__
	glfwInit();
	#if GLFW_VERSION_MAJOR == 3
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwwindow = glfwCreateWindow(width, height, "nontetris", gopt.fullscreen?glfwGetPrimaryMonitor():NULL, NULL);
	glfwMakeContextCurrent(glfwwindow);
	#else
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
	glfwOpenWindow(width, height, 5, 6, 5, 8, 0, 0, gopt.fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );
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

	glDisable(GL_DEPTH_TEST);

	GLuint vs, /* Vertex Shader */
	       fs, /* Fragment Shader */
	       ivs,
	       compfs;




	#ifdef __DUETTO__
		auto * c_vsSource = fileloader.getfilecontent("shader.vert");
		auto * c_ivsSource = fileloader.getfilecontent("shaderident.vert");
		auto * c_fsSource = client::String(WEBPREAMBLE).concat(fileloader.getfilecontent("shader.frag"));
		auto * c_compfsSource = client::String(WEBPREAMBLE).concat(fileloader.getfilecontent("shadercomp.frag"));
	#else
		std::string vsSource = FileLoader::getfilecontent(DATAPATHPREAMBLE "shader.vert");
		std::string ivsSource = FileLoader::getfilecontent(DATAPATHPREAMBLE "shaderident.vert");
		std::string fsSource = FileLoader::getfilecontent(DATAPATHPREAMBLE "shader.frag");
		std::string compfsSource = FileLoader::getfilecontent(DATAPATHPREAMBLE "shadercomp.frag");
		#ifdef EMSCRIPTEN
		fsSource = std::string(WEBPREAMBLE) + fsSource;
		compfsSource = std::string(WEBPREAMBLE) + compfsSource;
		#endif
		const char * c_vsSource = vsSource.c_str();
		const char * c_fsSource = fsSource.c_str();
		const char * c_ivsSource = ivsSource.c_str();
		const char * c_compfsSource = compfsSource.c_str();
	#endif

	vs = glCreateShader(GL_VERTEX_SHADER);
	#ifdef __DUETTO__
	webGLES->shaderSource(webGLESLookupWebGLShader(vs), *c_vsSource);
	#else
	glShaderSource(vs, 1, &c_vsSource, NULL);
	#endif
	glCompileShader(vs);
	printLog(vs,"vertex shader:");

	ivs = glCreateShader(GL_VERTEX_SHADER);
	#ifdef __DUETTO__
	webGLES->shaderSource(webGLESLookupWebGLShader(ivs), *c_ivsSource);
	#else
	glShaderSource(ivs, 1, &c_ivsSource, NULL);
	#endif
	glCompileShader(ivs);
	printLog(ivs,"ident vertex shader:");

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	#ifdef __DUETTO__
	webGLES->shaderSource(webGLESLookupWebGLShader(fs), *c_fsSource);
	#else
	glShaderSource(fs, 1, &c_fsSource, NULL);
	#endif
	glCompileShader(fs);
	printLog(fs, "fragment shader:");

	compfs = glCreateShader(GL_FRAGMENT_SHADER);
	#ifdef __DUETTO__
	webGLES->shaderSource(webGLESLookupWebGLShader(compfs), *c_compfsSource);
	#else
	glShaderSource(compfs, 1, &c_compfsSource, NULL);
	#endif
	glCompileShader(compfs);
	printLog(compfs, "completeness fragment shader:");

	sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	printLog(sp, "linking piece shader:");

	isp = glCreateProgram();
	glAttachShader(isp, ivs);
	glAttachShader(isp, fs);
	glLinkProgram(isp);
	printLog(isp, "linking global shader:");

	compsp = glCreateProgram();
	glAttachShader(compsp, ivs);
	glAttachShader(compsp, compfs);
	glLinkProgram(compsp);
	printLog(compsp, "linking completeness shader:");

	glUseProgram(isp);
	aGlobalVertexPositionLoc = glGetAttribLocation(isp, "aVertexPosition");
	aGlobalTextureCoordLoc = glGetAttribLocation(isp, "aTextureCoord");

	glUseProgram(sp);
	uPMatrixLoc = glGetUniformLocation(sp, "uPMatrix");
	uRTVecLoc = glGetUniformLocation(sp, "uRTVec");
	aVertexPositionLoc = glGetAttribLocation(sp, "aVertexPosition");

	glUseProgram(compsp);
	aCompVertexPositionLoc = glGetAttribLocation(compsp, "aVertexPosition");
	aCompTextureCoordLoc = glGetAttribLocation(compsp, "aTextureCoord");
	uCompLoc = glGetUniformLocation(compsp, "uComp");

	glUseProgram(sp);

	GLfloat PMatrix[16];

	//ortho(PMatrix,0,10.25,18,0,-1,1); // Without lateral background
	//ortho(PMatrix,-2.05,18.45,18,0,-1,1); //With noncutting background
	ortho(PMatrix,-1.75,18.25,18,0,-1,1); //With cutting background
	glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);

	//TEXTURE

	glGenTextures(1, &tex_background);
	glBindTexture( GL_TEXTURE_2D, tex_background);
	unsigned int twidth, theight;

	#ifndef __DUETTO__
	unsigned char * image;
	lodepng_decode24_file(&image, &twidth, &theight, DATAPATHPREAMBLE "imgs/newgamebackgroundgamea.png");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image );
	free(image);
	#else
	webGLES->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById("imgs/newgamebackgroundgamea.png")));
	#endif
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glGenerateMipmap( GL_TEXTURE_2D );

	glUseProgram(isp);

	GLuint tex_small[7];
	glGenTextures(7, tex);
	glGenTextures(7, tex_small);

	GLuint vbo_ident;
	glGenBuffers(1, &vbo_ident);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ident);

	GLfloat vert_ident [] = {
		-1, -1, 0, 0,
		-1, 1, 0, 1,
		1, -1, 1, 0,
		1, 1, 1, 1
	};
	glBufferData(GL_ARRAY_BUFFER, 4*2*2*sizeof(float), vert_ident, GL_STATIC_DRAW);

	for (int i = 0; i < 7; i++)
	{
		glBindTexture( GL_TEXTURE_2D, tex_small[i]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		#ifndef __DUETTO__
		#ifdef __MINGW32__
		//Mingw still doesn't have std::to_string
		char ctmp = '0'+(i+1);
		std::string path = std::string(DATAPATHPREAMBLE "imgs/pieces/")+ctmp+std::string(".png");
		#else
		std::string path = std::string(DATAPATHPREAMBLE "imgs/pieces/")+std::to_string(i+1)+std::string(".png");
		#endif

		lodepng_decode24_file(&image, &twidth, &theight, path.c_str());

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image );
		free(image);
		#else
		client::String prefixname("imgs/pieces/");
		client::String * pname = prefixname.concat(i+1);
		client::String * idname = pname->concat(".png");
		webGLES->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById(*idname)));
		#endif

		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		int piecefbosize = findsmallestpot(piecesAA*4*imgquad);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, piecefbosize, piecefbosize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);

		glGenFramebuffers(1, &(pieces_fbo[i]));
		glBindFramebuffer(GL_FRAMEBUFFER, pieces_fbo[i]);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[i], 0);
		glViewport(0, 0, piecefbosize, piecefbosize);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear( GL_COLOR_BUFFER_BIT );

		glEnableVertexAttribArray(aGlobalVertexPositionLoc);
		glEnableVertexAttribArray(aGlobalTextureCoordLoc);

		glBindTexture(GL_TEXTURE_2D, tex_small[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_ident);
		glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
		glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(aGlobalVertexPositionLoc);
		glDisableVertexAttribArray(aGlobalTextureCoordLoc);
		
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glGenerateMipmap(GL_TEXTURE_2D);


	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glDeleteTextures(7, tex_small); //I should delete textures, but it doesn't work on duetto

	glGenBuffers(1, &vbo_background);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_background);
	float rapx = 256.0/160.0;
	float rapy = 256.0/144.0;
	GLfloat vertices2 [] = {-1, -1,  0, 1/rapy,
				-1,  1,  0, 0,
				 1, -1,  1/rapx, 1/rapy,
				 1,  1,  1/rapx, 0};
	glBufferData(GL_ARRAY_BUFFER, 4*2*2*sizeof(float), vertices2, GL_STATIC_DRAW);

	//FONT
	glGenTextures(1, &tex_font);
	glBindTexture( GL_TEXTURE_2D, tex_font);

	#ifndef __DUETTO__
	lodepng_decode24_file(&image, &twidth, &theight, DATAPATHPREAMBLE "imgs/font.png");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image );
	free(image);
	#else
	webGLES->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById("imgs/font.png")));
	#endif
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glGenerateMipmap( GL_TEXTURE_2D );
	std::fill(vbo_score_num_vertices.begin(), vbo_score_num_vertices.end(), 0);

	glGenBuffers(1, &vbo_completeness);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_completeness);
	std::vector<GLfloat> vertices_side;
	for(float i = 0.0; i < rows; i += rowwidth)
	{
		vertices_side.insert(vertices_side.end(),
			{-1,                1.0F - i * (2.0F/rows),             0, 1,
			 -1,                1.0F - (i+rowwidth) *(2.0F/rows),   0, 0,
			 -1 + 6*(2.0F/160), 1.0F - i * (2.0F/rows),             1, 1,
			 -1 + 6*(2.0F/160), 1.0F - (i+rowwidth) *(2.0F/rows),   1, 0,  });
	}
	glBufferData(GL_ARRAY_BUFFER, vertices_side.size()*sizeof(float), vertices_side.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_lines);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
	std::vector<GLfloat> vertices_lines;
	for(float i = 0.0; i < rows; i += rowwidth)
	{
		vertices_lines.insert(vertices_lines.end(),
			{-1 + 14*(2.0F/160), 1.0F - i * (2.0F/rows),             0, 1,
			 -1 + 14*(2.0F/160), 1.0F - (i+rowwidth) *(2.0F/rows),   0, 0,
			 -1 + 96*(2.0F/160), 1.0F - i * (2.0F/rows),             1, 1,
			 -1 + 96*(2.0F/160), 1.0F - (i+rowwidth) *(2.0F/rows),   1, 0,  });
	}
	glBufferData(GL_ARRAY_BUFFER, vertices_lines.size()*sizeof(float), vertices_lines.data(), GL_STATIC_DRAW);
}

template <class vec>
static inline int string_to_vbo_vector(const std::string & s, vec & coords, int rightx, int bottomy)
{
	int index = 0;
	for (auto i = s.rbegin(); i != s.rend(); ++i)
	{
		const auto & c = *i;
		//Only numbers are supported
		int digit = c - '0';
		if (!(0 <= digit && digit <= 9))
			continue;
		coords.insert(coords.end(),{
			(rightx - index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,     (9+(8.0F*digit))/512, 1,
			(rightx - index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,           (9+(8.0F*digit))/512, 0,
			(rightx -1 - index) * (2/20.0F) -1, (17 - bottomy) * (2/18.0F) -1,  (1+(8.0F*digit))/512, 1,
			(rightx -1 - index) * (2/20.0F) -1, (18 - bottomy) * (2/18.0F) -1,        (1+(8.0F*digit))/512, 0
		});
		++index;
	}
	return index;
}

void GraphicHandler::updatescore(int number_a, int number_b, int number_c)
{
	if(glIsBuffer(vbo_score))
		glDeleteBuffers(1, &vbo_score);

	glGenBuffers(1, &vbo_score);

	std::string string_a = std::to_string(number_a);
	std::string string_b = std::to_string(number_b);
	std::string string_c = std::to_string(number_c);

	std::vector<float> coords;

	vbo_score_num_vertices[0] = string_to_vbo_vector(string_a, coords, 18, 10) * 4;
	vbo_score_num_vertices[1] = string_to_vbo_vector(string_b, coords, 18, 7) * 4;
	vbo_score_num_vertices[2] = string_to_vbo_vector(string_c, coords, 19, 3) * 4;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_score);
	glBufferData(GL_ARRAY_BUFFER, coords.size()*sizeof(float), coords.data(), GL_STATIC_DRAW);
}

GraphicHandler::~GraphicHandler()
{
	#ifndef __DUETTO__
	#if GLFW_VERSION_MAJOR == 3
	glfwDestroyWindow(glfwwindow);
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
GraphicPiece * GraphicHandler::createpiece(piece<float> pie)
{
	GraphicPiece * pgp = new GraphicPiece;
	GLuint VBOid;
	size_t size = (pie.totsize()+(pie.size()-1))*2;
	std::vector<GLfloat> vertices;
	vertices.reserve(size);
	bool firstshape = true;
	for ( auto pol: pie )
	{
		if ( firstshape )
		{
			firstshape = false;
		}
		else
		{
			vertices.push_back(pol[0].x);
			vertices.push_back(pol[0].y);
		}
		vertices.push_back(pol[0].x);
		vertices.push_back(pol[0].y);
		//std::cerr<<0<<std::endl;

		for ( unsigned int i = 1; i <= pol.size()/2; ++i)
		{
			auto & vert = pol[i];

			vertices.push_back(vert.x);
			vertices.push_back(vert.y);
			//std::cerr<<i<<std::endl;
			if(i < (pol.size()+1)/2)
			{
				auto & vert = pol[pol.size()-i];
				//std::cerr<<pol.size()-i<<std::endl;
				vertices.push_back(vert.x);
				vertices.push_back(vert.y);
			}
		}
	}

	glGenBuffers(1, &VBOid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOid);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

	pgp->VBOid = VBOid;
	pgp->num = static_cast<int>(size/2);
	pgp->tex = tex[pie.getType()];
	//std::cerr<<"NUM:"<<gp.num<<std::endl;
	return pgp;
}

void GraphicHandler::deletepiece(GraphicPiece * pgp)
{
	glDeleteBuffers(1,&(pgp->VBOid));
	delete pgp;
}

void GraphicHandler::beginrender()
{
	glViewport(0, 0, width, height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear( GL_COLOR_BUFFER_BIT );

	//DRAW BACKGROUND
	glUseProgram(isp);
	glEnableVertexAttribArray(aGlobalVertexPositionLoc);
	glEnableVertexAttribArray(aGlobalTextureCoordLoc);

	// BACKGROUND IMAGE
	glBindBuffer(GL_ARRAY_BUFFER, vbo_background);
	glBindTexture(GL_TEXTURE_2D, tex_background);
	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// TEXT
	glBindBuffer(GL_ARRAY_BUFFER, vbo_score);
	glBindTexture(GL_TEXTURE_2D, tex_font);
	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	int sum = 0;
	for (int i: vbo_score_num_vertices)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, sum, i);
		sum += i;
	}

	glDisableVertexAttribArray(aGlobalVertexPositionLoc);
	glDisableVertexAttribArray(aGlobalTextureCoordLoc);

	//END DRAW BACKGROUND

	glUseProgram(sp);
	glEnableVertexAttribArray(aVertexPositionLoc);
}

void GraphicHandler::renderpiece(float x, float y, float rot, GraphicPiece * gp)
{
	GLfloat RTVec[4] = {(GLfloat)sin(rot), (GLfloat)cos(rot), x, y};
	glUniform4fv(uRTVecLoc, 1, RTVec);

	glBindTexture( GL_TEXTURE_2D, gp->tex);
	glBindBuffer(GL_ARRAY_BUFFER, gp->VBOid);

	glVertexAttribPointer(aVertexPositionLoc, 2, GL_FLOAT, false, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, gp->num);
}

void GraphicHandler::endrender(const std::vector<float> & linecompleteness, const std::vector<bool> & linecutblack)
{
	glDisableVertexAttribArray(aVertexPositionLoc);

	// LINE COMPLETENESS

	glUseProgram(compsp);
	glEnableVertexAttribArray(aCompVertexPositionLoc);
	glEnableVertexAttribArray(aCompTextureCoordLoc);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_completeness);
	glVertexAttribPointer(aCompVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aCompTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	int lineind = 0;
	for(float i = 0.0; i < rows; i += rowwidth, ++lineind)
	{
		glUniform1f(uCompLoc, linecompleteness[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, lineind*4, 4);
	}
	// LINE CUT BLACK
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
	glVertexAttribPointer(aCompVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aCompTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
	lineind = 0;
	for(float i = 0.0; i < rows; i += rowwidth, ++lineind)
	{
		if(linecutblack[i])
		{
			glUniform1f(uCompLoc, 1.0);
			glDrawArrays(GL_TRIANGLE_STRIP, lineind*4, 4);
		}
	}


	glDisableVertexAttribArray(aCompVertexPositionLoc);
	glDisableVertexAttribArray(aCompTextureCoordLoc);


	#ifndef __DUETTO__
	#if GLFW_VERSION_MAJOR == 3
	glfwSwapBuffers(glfwwindow);
	glfwPollEvents();
	#else
	glfwSwapBuffers();
	#endif
	#endif
}
