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

#include "gameoptions.h"

#include <vector>
#include <memory>

class GameHandler
{
	struct GamePiece
	{
		PhysicPiece * php;
		GraphicPiece * grp;
		piece<float> p;
		bool graphicshared;
		GamePiece(const piece<float> &_p, bool _graphicshared = false):p(_p), graphicshared(_graphicshared)
		{
		}
	};

	PhysicHandler * phphysic;
	GraphicHandler * phgraphic;
	InputHandler * phinput;

	void newrandompiece();
	GamePiece * newpiece(const piece<float> & p, float x, float y, float rot, PhysicPiece::PhysicPieceType type, GraphicPiece * sharedgp);
	void randomnextpiece();
	void deletepiece(GamePiece *);
	void cutline(float from, float to);
	float computelinearea(float from, float to);
	void togglepause();
	void updatelinearea();

	const GameOptions gameopt;

	//State of the game
	int score;
	int level;
	int linesortiles;
	enum GameState {RUNNING, GAMEOVER, CUTPAUSED, PAUSED, CUTPAUSED_PAUSED} gamestate;
	int cutpausecontdown;
	std::vector<bool> linesbeingcut;
	std::vector<bool> linesfalse;

	//This is a cache and is used only to avoid recreating the same textures
	std::vector<std::unique_ptr<GraphicPiece>> graphicpieces_uncutted;

	int nextpiece_type;
	float nextpiece_rot;
	std::vector<float> linearea;
	std::vector<float> linecompleteness;
	float updatebarscompleteness;

public:
	GameHandler(const GameOptions & _gameopt, const FileLoader & fl);
	void step_physic();
	void step_graphic();
	bool step_logic();
	~GameHandler();
};

#endif //GAMEHANDLER_H
