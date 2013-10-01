#ifndef _GRAPHIC_HANDLER_H
#define _GRAPHIC_HANDLER_H

#ifdef __DUETTO__
#include "duettogl.h"
#else
#include <GL/glew.h>
#endif

#include "polygon.h"
#include "piece.h"

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
	GLint uPMatrixLoc;
	GLint uRTVecLoc;
	GLint uMyTexLoc;
	GLint aVertexPositionLoc;
	GLuint tex_background;
	GLuint tex_small[7];
	GLuint tex[7];

	GLuint tex_fbo;
	GLuint fbo;
	GLuint pieces_fbo[7];
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
	bool render(std::function< void(std::function<void(float x, float y, float rot, GraphicPiece * d)>)>allbodies );
};


#endif //_GRAPHIC_HANDLER_H
