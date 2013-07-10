#include "graphichandler.h"

#include <algorithm>
#include <cmath>
#include <fstream>

#include <GL/glew.h>
#include <GL/glfw.h>

void printLog(GLuint obj)
{
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);

	if (infologLength > 0)
		std::cout << infoLog << std::endl;
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

GraphicHandler::GraphicHandler(int width, int height, bool fullscreen)
{
	glfwInit();
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,8);
	glfwOpenWindow(width, height, 5, 6, 5, 8, 0, 0, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );


	glViewport(0, 0, width, height);

	if(glewInit() != GLEW_OK)
		std::cerr<<"GLEW fail"<<std::endl;

	GLuint vs, /* Vertex Shader */
	       fs, /* Fragment Shader */
	       sp; /* Shader Program */


	std::string vsSource = file2string("../shader.vert");
	const char * c_vsSource = vsSource.c_str();
	std::string fsSource = file2string("../shader.frag");
	const char * c_fsSource = fsSource.c_str();

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, (const GLchar **) &c_vsSource, NULL);
	glCompileShader(vs);
	printLog(vs);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, (const GLchar **) &c_fsSource, NULL);
	glCompileShader(fs);
	printLog(fs);

	sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	printLog(sp);

	glUseProgram(sp);

	uPMatrixLoc = glGetUniformLocation(sp, "uPMatrix");
	uRTVecLoc = glGetUniformLocation(sp, "uRTVec");
	aVertexPositionLoc = glGetAttribLocation(sp, "aVertexPosition");
	glEnableVertexAttribArray(aVertexPositionLoc);

	GLfloat PMatrix[16];
	ortho(PMatrix,0,10.25,18,0,-1,1);
	glUniformMatrix4fv(uPMatrixLoc, 1, false, PMatrix);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
	pgp->num = size/2;
	//std::cerr<<"NUM:"<<gp.num<<std::endl;
	return pgp;
}

bool GraphicHandler::render(std::function< void(std::function<void(float x, float y, float rot, void * d)>)>allbodies )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	allbodies([this](float x, float y, float rot, void * d)
	{
		GraphicPiece & gp = *(GraphicPiece * )d;
		//std::cerr <<"(" << x << ", " << y << "); rot:"<< rot<<std::endl;
		//std::cerr <<" VBO:" << gp.VBOid << " num:"<< gp.num << std::endl;

		GLfloat RTVec[4] = {(GLfloat)sin(rot), (GLfloat)cos(rot), x, y};
		glUniform4fv(uRTVecLoc, 1, RTVec);

		glBindBuffer(GL_ARRAY_BUFFER_ARB, gp.VBOid);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexAttribPointer(aVertexPositionLoc, 2, GL_FLOAT, false, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, gp.num);

		//glDisableClientState(GL_VERTEX_ARRAY);
		//glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	});

	glfwSwapBuffers();
	return true;
}
