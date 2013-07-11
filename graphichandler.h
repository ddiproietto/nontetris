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
	GLuint tex[7];
public:
	GraphicHandler(int width = 800, int height = 600, bool fullscreen = false);
	~GraphicHandler();
	GraphicPiece * createpiece(piece<float> pol);
	bool render(std::function< void(std::function<void(float x, float y, float rot, void * d)>)>allbodies );
};


#endif //_GRAPHIC_HANDLER_H
