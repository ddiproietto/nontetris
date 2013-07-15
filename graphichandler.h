#ifndef _GRAPHIC_HANDLER_H
#define _GRAPHIC_HANDLER_H

#include "polygon.h"
#include "piece.h"

#include <functional>

#include <GL/glew.h>

struct GraphicPiece
{
	GLuint VBOid;
	GLuint num;
	int tex;
};

class GraphicHandler
{
	GLint uPMatrixLoc;
	GLint uRTVecLoc;
	GLint uMyTexLoc;
	GLint aVertexPositionLoc;
	GLuint tex_background;
	GLuint tex[7];

	GLuint tex_fbo;
	GLuint fbo;
	GLuint vbo_main_rect;
	GLuint vbo_all_screen;

	GLfloat PMatrix[16];

	bool fbo_used;

	int width;
	int height;
	double fsaa;

	GLuint sp, gsp;
	GLint aGlobalVertexPositionLoc;
	GLint aGlobalTextureCoordLoc;

public:
	GraphicHandler(int width = 600, int height = 540, bool fullscreen = false);
	~GraphicHandler();
	GraphicPiece * createpiece(piece<float> pol);
	bool render(std::function< void(std::function<void(float x, float y, float rot, void * d)>)>allbodies );
};


#endif //_GRAPHIC_HANDLER_H
