#include "graphichandler.h"

#include "NontetrisConfig.h"

#ifdef __DUETTO__
#include "duettogl.h"
#include <duetto/client.h>
#include <duetto/clientlib.h>
#else

#ifndef EMSCRIPTEN
#define GLEW_NO_GLU
#include <GL/glew.h>
#endif

#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
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

GraphicHandler::GraphicHandler(int width, int height, bool fullscreen, FileLoader * fileloader):width(width), height(height)
{
	float imgquad = height/18.0;
	float piecesAA = 2;

	#ifndef __DUETTO__
	glfwInit();
	#if GLFW_VERSION_MAJOR == 3
	//TODO: honor fullscreen
	glfwwindow = glfwCreateWindow(width, height, "nontetris", NULL, NULL);
	glfwMakeContextCurrent(glfwwindow);
	#else
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
	glfwOpenWindow(width, height, 5, 6, 5, 8, 0, 0, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );
	#endif
		#ifndef EMSCRIPTEN
		glewExperimental = GL_TRUE;
		if(glewInit() != GLEW_OK)
			std::cerr<<"GLEW fail"<<std::endl;
		#endif //defined(__EMSCRIPTEN__)
	glEnableClientState(GL_VERTEX_ARRAY);
	#else //defined(__DUETTO__)
	duettoGLInit(width, height);
	#endif

	glDisable(GL_DEPTH_TEST);

	GLuint vs, /* Vertex Shader */
	       fs, /* Fragment Shader */
	       ivs;


	#ifdef __DUETTO__
		auto * c_vsSource = fileloader->getfilecontent("shader.vert");
		auto * c_fsSource = client::String(WEBPREAMBLE).concat(fileloader->getfilecontent("shader.frag"));
		auto * c_ivsSource = fileloader->getfilecontent("shaderident.vert");
	#else
		#if !defined(EMSCRIPTEN)
		std::string vsSource = FileLoader::getfilecontent("../shader.vert");
		std::string fsSource = FileLoader::getfilecontent("../shader.frag");
		std::string ivsSource = FileLoader::getfilecontent("../shaderident.vert");
		#else
		std::string vsSource = FileLoader::getfilecontent("shader.vert");
		std::string fsSource = std::string(WEBPREAMBLE) + FileLoader::getfilecontent("shader.frag");
		std::string ivsSource = FileLoader::getfilecontent("shaderident.vert");
		#endif
		const char * c_vsSource = vsSource.c_str();
		const char * c_fsSource = fsSource.c_str();
		const char * c_ivsSource = ivsSource.c_str();
	#endif

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &c_vsSource, NULL);
	glCompileShader(vs);
	printLog(vs,"vertex shader:");

	ivs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ivs, 1, &c_ivsSource, NULL);
	glCompileShader(ivs);
	printLog(vs,"ident vertex shader:");

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &c_fsSource, NULL);
	glCompileShader(fs);
	printLog(fs, "fragment shader:");

	sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	printLog(sp, "linking shader:");

	isp = glCreateProgram();
	glAttachShader(isp, ivs);
	glAttachShader(isp, fs);
	glLinkProgram(isp);
	printLog(isp, "linking shader:");

	glUseProgram(isp);
	aGlobalVertexPositionLoc = glGetAttribLocation(isp, "aVertexPosition");
	aGlobalTextureCoordLoc = glGetAttribLocation(isp, "aTextureCoord");

	glUseProgram(sp);
	uPMatrixLoc = glGetUniformLocation(sp, "uPMatrix");
	uRTVecLoc = glGetUniformLocation(sp, "uRTVec");
	aVertexPositionLoc = glGetAttribLocation(sp, "aVertexPosition");

	GLfloat PMatrix[16];

	//ortho(PMatrix,0,10.25,18,0,-1,1);
	ortho(PMatrix,-2.05,18.45,18,0,-1,1);
	glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);

	//TEXTURE

	glGenTextures(1, &tex_background);
	glBindTexture( GL_TEXTURE_2D, tex_background);
	unsigned int twidth, theight;

	#ifndef __DUETTO__
	unsigned char * image;
	lodepng_decode24_file(&image, &twidth, &theight, "../imgs/newgamebackground.png");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image );
	free(image);
	#else
	gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById("imgs/newgamebackground.png")));
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
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_ident);

	GLfloat vert_ident [] = {
		-1, -1, 0, 0,
		-1, 1, 0, 1,
		1, -1, 1, 0,
		1, 1, 1, 1
	};
	glBufferData(GL_ARRAY_BUFFER_ARB, 4*2*2*sizeof(float), vert_ident, GL_STATIC_DRAW_ARB);

	for (int i = 0; i < 7; i++)
	{
		glBindTexture( GL_TEXTURE_2D, tex_small[i]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		#ifndef __DUETTO__
			#ifndef EMSCRIPTEN
			std::string path = std::string("../imgs/pieces/")+std::to_string(i+1)+std::string(".png");
			#else
			std::string path = std::to_string(i+1)+std::string(".png");
			#endif //EMSCRIPTEN

		lodepng_decode24_file(&image, &twidth, &theight, path.c_str());

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image );
		free(image);
		#else
		client::String prefixname("imgs/pieces/");
		client::String * pname = prefixname.concat(i+1);
		client::String * idname = pname->concat(".png");
		gl->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<client::HTMLImageElement *>(client::document.getElementById(*idname)));
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

		glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_ident);
		glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
		glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(aGlobalVertexPositionLoc);
		glDisableVertexAttribArray(aGlobalTextureCoordLoc);
		
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glGenerateMipmap(GL_TEXTURE_2D);


	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &vbo_background);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_background);
	float rapx = 256.0/160.0;
	float rapy = 256.0/144.0;
	GLfloat vertices2 [] = {-1, -1,  0, 1/rapy,
				-1,  1,  0, 0,
				 1, -1,  1/rapx, 1/rapy,
				 1,  1,  1/rapx, 0};
	glBufferData(GL_ARRAY_BUFFER_ARB, 4*2*2*sizeof(float), vertices2, GL_STATIC_DRAW_ARB);

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

#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
GLFWwindow * GraphicHandler::getglfwwindow()
{
	return glfwwindow;
}
#endif

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
	glBindBuffer(GL_ARRAY_BUFFER_ARB, VBOid);
	glBufferData(GL_ARRAY_BUFFER_ARB, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW_ARB);

	pgp->VBOid = VBOid;
	pgp->num = static_cast<int>(size/2);
	pgp->tex = tex[pie.getType()];
	//std::cerr<<"NUM:"<<gp.num<<std::endl;
	return pgp;
}

bool GraphicHandler::render(const std::function< void(const std::function<void(float x, float y, float rot, GraphicPiece * d)>&)> & allbodies )
{
	glViewport(0, 0, width, height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear( GL_COLOR_BUFFER_BIT );

	//DRAW BACKGROUND
	glUseProgram(isp);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_background);
	glEnableVertexAttribArray(aGlobalVertexPositionLoc);
	glEnableVertexAttribArray(aGlobalTextureCoordLoc);

	glBindTexture(GL_TEXTURE_2D, tex_background);

	glVertexAttribPointer(aGlobalVertexPositionLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)0);
	glVertexAttribPointer(aGlobalTextureCoordLoc, 2, GL_FLOAT, false, 4*sizeof(GLfloat), (glvapt)(2*sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(aGlobalVertexPositionLoc);
	glDisableVertexAttribArray(aGlobalTextureCoordLoc);
	//END DRAW BACKGROUND

	glUseProgram(sp);
	glEnableVertexAttribArray(aVertexPositionLoc);

	allbodies([this](float x, float y, float rot, GraphicPiece * gp)
	{
		GLfloat RTVec[4] = {(GLfloat)sin(rot), (GLfloat)cos(rot), x, y};
		glUniform4fv(uRTVecLoc, 1, RTVec);

		glBindTexture( GL_TEXTURE_2D, gp->tex);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, gp->VBOid);

		glVertexAttribPointer(aVertexPositionLoc, 2, GL_FLOAT, false, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, gp->num);
	});
	glDisableVertexAttribArray(aVertexPositionLoc);

	#ifndef __DUETTO__
	#if GLFW_VERSION_MAJOR == 3
	glfwSwapBuffers(glfwwindow);
	glfwPollEvents();
	#else
	glfwSwapBuffers();
	#endif
	#endif

	return true;
}
