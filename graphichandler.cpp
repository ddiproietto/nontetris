#include "graphichandler.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>

#ifndef EMSCRIPTEN
#include <GL/glew.h>
#endif
#include <GL/glfw.h>

#include "SOIL/SOIL.h"

void printLog(GLuint obj, const std::string & str)
{
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);

	if (infologLength > 0)
		std::cout << str <<infoLog << std::endl;
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

std::string file2string(const std::string & filename)
{
	std::ifstream t(filename);
	if(!t)
	{
		std::cerr << "Error reading file "<< filename <<std::endl;
		return "";
	}
	std::string str;
	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}

GraphicHandler::GraphicHandler(int width, int height, bool fullscreen):width(width), height(height)
{
	glfwInit();
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,8);
	glfwOpenWindow(width, height, 5, 6, 5, 8, 0, 0, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );

	//glViewport(0, 0, width, height);

	#ifndef EMSCRIPTEN
	if(glewInit() != GLEW_OK)
		std::cerr<<"GLEW fail"<<std::endl;
	#endif

	GLuint vs, /* Vertex Shader */
	       fs, /* Fragment Shader */
	       sp; /* Shader Program */


	#ifndef EMSCRIPTEN
	std::string vsSource = file2string("../shader.vert");
	const char * c_vsSource = vsSource.c_str();
	std::string fsSource = file2string("../shader.frag");
	const char * c_fsSource = fsSource.c_str();
	#else
	std::string vsSource = file2string("wshader.vert");
	const char * c_vsSource = vsSource.c_str();
	std::string fsSource = file2string("wshader.frag");
	const char * c_fsSource = fsSource.c_str();
	#endif

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, (const GLchar **) &c_vsSource, NULL);
	glCompileShader(vs);
	printLog(vs,"vertex shader:");

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, (const GLchar **) &c_fsSource, NULL);
	glCompileShader(fs);
	printLog(fs, "fragment shader:");

	sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	printLog(sp, "linking shader:");

	glUseProgram(sp);

	uPMatrixLoc = glGetUniformLocation(sp, "uPMatrix");
	uRTVecLoc = glGetUniformLocation(sp, "uRTVec");
	aVertexPositionLoc = glGetAttribLocation(sp, "aVertexPosition");
	glEnableVertexAttribArray(aVertexPositionLoc);

	ortho(PMatrix,0,10.25,18,0,-1,1);
	eye(IMatrix);
	glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//TEXTURE

	uMyTexLoc = glGetUniformLocation(sp, "myTexture");
	
	glGenTextures(7, tex);

	for (int i = 0; i < 7; i++)
	{
		glBindTexture( GL_TEXTURE_2D, tex[i]);
		glActiveTexture(GL_TEXTURE0);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glGenerateMipmap( GL_TEXTURE_2D );
		int twidth, theight;
		#ifndef EMSCRIPTEN
		std::string path = std::string("../pieces/")+std::to_string(i+1)+std::string(".png");
		#else
		std::string path = std::to_string(i+1)+std::string(".png");
		#endif //EMSCRIPTEN
		unsigned char * image =
			SOIL_load_image(path.c_str() , &twidth, &theight, 0, SOIL_LOAD_RGB );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image );
		SOIL_free_image_data( image );
	}
	
	glDisable(GL_DEPTH_TEST);

	//ANTIALIASING
	fsaa = 2;
	glGenTextures(1, &tex_fbo);
	glBindTexture(GL_TEXTURE_2D, tex_fbo);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fsaa*width, fsaa*height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_fbo, 0);
	//WARNING:no depth test in the texture, should use another renderbuffer
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	fbo_used = (status == GL_FRAMEBUFFER_COMPLETE);
	if(!fbo_used)
		std::cerr << "error initializing fbo" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &vbo_main_rect);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_main_rect);
	GLfloat vertices [] = {0, 0, 0, 18, 10.25, 0, 10.25, 18};
	//GLfloat vertices [] = {0, 0, 0, 9, 5.125, 0, 5.125, 9};
	glBufferData(GL_ARRAY_BUFFER_ARB, 4*2*sizeof(float), vertices, GL_STATIC_DRAW_ARB);

}

GraphicHandler::~GraphicHandler()
{
	glfwTerminate();
}

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

		for ( int i = 1; i <= pol.size()/2; ++i)
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

bool GraphicHandler::render(std::function< void(std::function<void(float x, float y, float rot, void * d)>)>allbodies )
{
	glViewport(0, 0, fsaa*width, fsaa*height);
	if(fbo_used)
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear( GL_COLOR_BUFFER_BIT );
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear( GL_COLOR_BUFFER_BIT );

	allbodies([this](float x, float y, float rot, void * d)
	{
		GraphicPiece & gp = *(GraphicPiece * )d;
		//std::cerr <<"(" << x << ", " << y << "); rot:"<< rot<<std::endl;
		//std::cerr <<" VBO:" << gp.VBOid << " num:"<< gp.num << std::endl;

		GLfloat RTVec[4] = {(GLfloat)sin(rot), (GLfloat)cos(rot), x, y};
		glUniform4fv(uRTVecLoc, 1, RTVec);

		glBindTexture( GL_TEXTURE_2D, gp.tex);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, gp.VBOid);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexAttribPointer(aVertexPositionLoc, 2, GL_FLOAT, false, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, gp.num);

		//glDisableClientState(GL_VERTEX_ARRAY);
		//glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	});

	if(fbo_used)
	{
		glViewport(0, 0, width, height);

		GLfloat RTVec[4] = {(GLfloat)sin(0),(GLfloat)cos(0),5,0};
		glUniform4fv(uRTVecLoc, 1, RTVec);
		glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, tex_fbo);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_main_rect);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexAttribPointer(aVertexPositionLoc, 2, GL_FLOAT, false, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glfwSwapBuffers();
	return true;
}
