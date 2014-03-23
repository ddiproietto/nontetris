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
#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include "graphichandler.h"
#include "physichandler.h"
#include "inputhandler.h"

#include <list>

struct GameOptions
{
	double rows;
	double columns;
};

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

	GameOptions gameopt;

public:
	GameHandler(const GraphicOptions & gopt, const GameOptions & _gameopt, const FileLoader & fl, double physicstep);
	void step_physic();
	void step_graphic();
	bool step_logic();
	~GameHandler();
};

#endif //GAMEHANDLER_H
