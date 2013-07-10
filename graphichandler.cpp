#include "graphichandler.h"

#include <algorithm>

#include <GL/glew.h>
#include <GL/glfw.h>

GraphicHandler::GraphicHandler(int width, int height, bool fullscreen)
{
	glfwInit();
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,8);
	glfwOpenWindow(width, height, 5, 6, 5, 8, 0, 0, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW );


	glViewport(0, 0, width, height);

	if(glewInit() != GLEW_OK)
		std::cerr<<"GLEW fail"<<std::endl;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,10.25,18,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	size_t size = (pie.totsize()+(pie.size()-1))*3;
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
			vertices.push_back(0);
		}
		vertices.push_back(pol[0].x);
		vertices.push_back(pol[0].y);
		vertices.push_back(0);
		//std::cerr<<0<<std::endl;

		for ( int i = 1; i <= pol.size()/2; ++i)
		{
			auto & vert = pol[i];

			vertices.push_back(vert.x);
			vertices.push_back(vert.y);
			vertices.push_back(0);
			//std::cerr<<i<<std::endl;
			if(i < (pol.size()+1)/2)
			{
				auto & vert = pol[pol.size()-i];
				//std::cerr<<pol.size()-i<<std::endl;
				vertices.push_back(vert.x);
				vertices.push_back(vert.y);
				vertices.push_back(0);
			}
		}
	}

	glGenBuffers(1, &VBOid);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, VBOid);
	glBufferData(GL_ARRAY_BUFFER_ARB, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW_ARB);

	pgp->VBOid = VBOid;
	pgp->num = size/3;
	//std::cerr<<"NUM:"<<gp.num<<std::endl;
	return pgp;
}

bool GraphicHandler::render(std::function< void(std::function<void(float x, float y, float rot, void * d)>)>allbodies )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	allbodies([](float x, float y, float rot, void * d)
	{
		GraphicPiece & gp = *(GraphicPiece * )d;
		//std::cerr <<"(" << x << ", " << y << "); rot:"<< rot<<std::endl;
		//std::cerr <<" VBO:" << gp.VBOid << " num:"<< gp.num << std::endl;

		glLoadIdentity();
		glTranslatef(x,y,0);
		glRotatef(rot*180.0/M_PI, 0, 0, 1);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, gp.VBOid);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, gp.num);

		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	});

	glfwSwapBuffers();
	return true;
}
