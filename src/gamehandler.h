#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include "graphichandler.h"
#include "physichandler.h"
#include "inputhandler.h"

#include <list>

class GameHandler
{
	struct GamePiece
	{
		PhysicPiece * php;
		GraphicPiece * grp;
	};

	std::list<GamePiece> ingamepieces;

	PhysicHandler * phphysic;
	GraphicHandler * phgraphic;
	InputHandler * phinput;

	void newrandompiece();

public:
	GameHandler(const GraphicOptions & gopt, const FileLoader & fl, float width=10.25, float height=18);
	bool oneiteration();
	~GameHandler();
};

#endif //GAMEHANDLER_H
