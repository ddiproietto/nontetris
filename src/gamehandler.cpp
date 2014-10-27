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
#include "gamehandler.h"

#include <list>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#define DEBUGAREALOSS
#ifdef DEBUGAREALOSS
#include <csignal>
#endif

#include "myutil.h"

#include "cutter.h"

namespace
{
	auto pieces = make_array(
			piece <float> ({ {-2, -0.5}, {-1, -0.5}, {1, -0.5}, {2, -0.5}, {2, 0.5}, {1, 0.5}, {-1, 0.5}, {-2, 0.5} }, 0), //I
			piece <float> ({ {-1.5, -1}, {-0.5, -1}, {0.5, -1}, {1.5, -1}, {1.5, 0}, {1.5, 1}, {0.5, 1}, {0.5, 0}, {-0.5, 0}, {-1.5, 0} }, 1), //J
			piece <float> ({ {-1.5, -1}, {-0.5, -1}, {0.5, -1}, {1.5, -1}, {1.5, 0}, {0.5, 0}, {-0.5, 0}, {-0.5, 1}, {-1.5, 1}, {-1.5, 0} }, 2), //L
			piece <float> ({ {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} }, 3), //O
			piece <float> ({ {-0.5, -1}, {0.5, -1}, {1.5, -1}, {1.5, 0}, {0.5, 0}, {0.5, 1}, {-0.5, 1}, {-1.5, 1}, {-1.5, 0}, {-0.5, 0} }, 4), //S
			piece <float> ({ {-1.5, -1}, {-0.5, -1}, {0.5, -1}, {0.5, 0}, {1.5, 0}, {1.5, 1}, {0.5, 1}, {-0.5, 1}, {-0.5, 0}, {-1.5, 0} }, 5), //Z
			piece <float> ({ {-1.5, -1}, {-0.5, -1}, {0.5, -1}, {1.5, -1}, {1.5, 0}, {0.5, 0}, {0.5, 1}, {-0.5, 1}, {-0.5, 0}, {-1.5, 0} }, 6) //T
			);
}

GameHandler::GameHandler(const GameOptions &_gameopt, const FileLoader &fl, const TextureLoader &tl):
physichandler(_gameopt), graphichandler (_gameopt, fl, tl), inputhandler(graphichandler.toinput()),
gameopt(_gameopt),score(0),level(0),linesortiles(0),gamestate(RUNNING), nextpiece_rot(0.0), updatebarscompleteness(1.0)
{
	for (const auto & p: pieces)
	{
		// Should use make_unique, but it's c++1y
		graphicpieces_uncutted.push_back(std::move(std::unique_ptr<GraphicPiece>(graphichandler.createpiece(p))));
	}

	textscores[0] = texthandler.createtextfragment(std::to_string(linesortiles), TextAlign::ALIGN_RIGHT, 17, 10);
	textscores[1] = texthandler.createtextfragment(std::to_string(level), TextAlign::ALIGN_RIGHT, 17, 7);
	textscores[2] = texthandler.createtextfragment(std::to_string(score), TextAlign::ALIGN_RIGHT, 18, 3);

	for (float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
	{
		linecompleteness.push_back(0.0);
		linearea.push_back(0.0);
		linesbeingcut.push_back(false);
		linesfalse.push_back(false);
	}
	//Start the game with a new random piece!
	randomnextpiece();
	newrandompiece();
}

GameHandler::~GameHandler()
{
}

void GameHandler::togglepause()
{
	if(gamestate == GAMEOVER)
		return;
	if(gamestate == RUNNING)
		gamestate = PAUSED;
	else if (gamestate == PAUSED)
		gamestate = RUNNING;
	else if (gamestate == CUTPAUSED)
		gamestate = CUTPAUSED_PAUSED;
	else if (gamestate == CUTPAUSED_PAUSED)
		gamestate = CUTPAUSED;
}

GameHandler::GamePiece * GameHandler::newpiece(const piece<float> & p, float x, float y, float rot, PhysicPiece::PhysicPieceType type, GraphicPiece * sharedgp = NULL)
{
	//TODO: free this memory at exit
	auto * gamepiece = new GamePiece(p);
	gamepiece->php = physichandler.createpiece(p, x, y, rot, gamepiece, type, level);
	if (sharedgp)
	{
		gamepiece->graphicshared = true;
		gamepiece->grp = sharedgp;
	}
	else
	{
		gamepiece->graphicshared = false;
		gamepiece->grp = graphichandler.createpiece(p);
	}
	return gamepiece;
}

void GameHandler::deletepiece(GamePiece* pgp)
{
	if (!pgp->graphicshared)
		graphichandler.deletepiece(pgp->grp);
	physichandler.destroypiece(pgp->php);
	delete pgp;
}

void GameHandler::randomnextpiece()
{
//Different ways of generating random numbers
#if defined (__CHEERP__)
	int randpieceindex = int(client::Math.random()*pieces.size());
#elif defined (EMSCRIPTEN)
	int randpieceindex = rand()%pieces.size();
#else
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, pieces.size()-1);

	int randpieceindex = dis(gen);
#endif

	nextpiece_type = randpieceindex;
}

void GameHandler::newrandompiece()
{
//Move nextpiece to falling
	newpiece(pieces[nextpiece_type], gameopt.columns/2, -1, 0.0, PhysicPiece::FALLING_PIECE, graphicpieces_uncutted[nextpiece_type].get());
//Create randomnextpiece
	randomnextpiece();
}

bool isugly(const polygon <float> & pol)
{
	if (pol.size() < 3)
		return true;
	//Add other ugly conditions
	return false;
}

void GameHandler::cutline(float from, float to)
{
	float x0 = 0;
	float y0 = from;
	float x1 = gameopt.columns;
	float y1 = to;

	struct DeletePiece
	{
		std::list<polygon<float>> remainders;
		GamePiece * pgp;
		int originaltype;
		float x,y,rot;
#ifdef DEBUGAREALOSS
		std::list<polygon<float>> midremainders;
#endif
	};
	std::list<DeletePiece> deletelist;

	physichandler.getpieces_in_rect(x0, y0, x1, y1, [y0, y1, &deletelist](PhysicPiece * php){
		polygon<float> p (static_cast<GamePiece *>(php->getUserData())->p.getshape());
		struct DeletePiece dp;

		// Transform the piece coordinates from local to global
		for (auto & vertex: p)
		{
			vertex.rotate(php->getRot());
			vertex.translate(php->getX(), php->getY());
		}

#ifndef DEBUGAREALOSS
		struct
		{
			void push_back(const polygon<float> & p)
			{
				(void) p;
			}
		} midcontainer;
#else
		auto & midcontainer = dp.midremainders;
#endif

		// Here cutter uses the tolerance
		bool isinbetween = cutter(p, dp.remainders, dp.remainders, midcontainer, y0, y1, 0.1F);

		if (isinbetween)
		{
			dp.pgp = static_cast<GamePiece *>(php->getUserData());
			dp.originaltype = dp.pgp->p.getType();
			dp.x = php->getX();
			dp.y = php->getY();
			dp.rot = php->getRot();
			deletelist.push_back(std::move(dp));
		}
	});

	for (auto & dp: deletelist)
	{
		GamePiece * pgp = dp.pgp;
#ifdef DEBUGAREALOSS
		float prevarea = pgp->p.getshape().area(),
			newarea = 0;
#endif

		// Create remainders
		for (auto & pol: dp.remainders)
		{
			// Transform the piece coordinates from global to local again
			for (auto & vertex: pol)
			{
				vertex.translate(-dp.x, -dp.y);
				vertex.rotate(-dp.rot);
			}
			if (isugly(pol))
				continue;
			piece<float> newp (pol, dp.originaltype);
			if (!newp.empty())
				newpiece(newp, dp.x, dp.y, dp.rot, PhysicPiece::OLD_PIECE);
#ifdef DEBUGAREALOSS
			newarea += pol.area();
#endif
		}
#ifdef DEBUGAREALOSS
		for (auto & pol: dp.midremainders)
		{
			newarea += pol.area();
		}
		if (fabs(prevarea - newarea) > 1.0)
			raise(SIGTRAP);
#endif
		deletepiece(pgp);
	}
}

//Returns the area of the mid part
float GameHandler::computelinearea(float from, float to)
{
	float x0 = 0;
	float y0 = from;
	float x1 = gameopt.columns;
	float y1 = to;
	float retarea = 0.0F;

	std::list<polygon<float>> midremainders;

	physichandler.getpieces_in_rect(x0, y0, x1, y1, [y0, y1, &midremainders](PhysicPiece * php){
		polygon<float> p (static_cast<GamePiece *>(php->getUserData())->p.getshape());

		// Transform the piece coordinates from local to global
		for (auto & vertex: p)
		{
			vertex.rotate(php->getRot());
			vertex.translate(php->getX(), php->getY());
		}

		struct
		{
			void push_back(const polygon<float> & p)
			{
				(void) p;
			}
		} dummycontainer;

		// Uses the cutter with no tolerance
		// I'm not interested here in up & down remainders
		cutter(p, dummycontainer, dummycontainer, midremainders, y0, y1, 0.0F);
	});

	for (const auto & midp: midremainders)
	{
		retarea += midp.area();
	}

	return retarea;
}

void GameHandler::updatelinearea()
{
	for (float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
	{
		linearea[i] = computelinearea(i, i + gameopt.rowwidth);
		linecompleteness[i] = std::min(linearea[i]/gameopt.cuttingrowarea, 1.0);
	}
}

void GameHandler::updatescoregraphic()
{
	for (const auto & i: textscores)
	{
		texthandler.deletetextfragment(i);
	}
	textscores[0] = texthandler.createtextfragment(std::to_string(linesortiles), TextAlign::ALIGN_RIGHT, 17, 10);
	textscores[1] = texthandler.createtextfragment(std::to_string(level), TextAlign::ALIGN_RIGHT, 17, 7);
	textscores[2] = texthandler.createtextfragment(std::to_string(score), TextAlign::ALIGN_RIGHT, 18, 3);
	// Note really necessary, but better doing it here than during rendering
	texthandler.updatevbo();
}

void GameHandler::step_physic()
{
	if (gamestate == PAUSED || gamestate == CUTPAUSED_PAUSED)
		return;
	else if (gamestate == CUTPAUSED)
	{
		if (--cutpausecontdown)
			return;
		else
		{
			gamestate = RUNNING;
			// Effectively cut lines at the end of the cutpause
			for (float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
			{
				if (linesbeingcut[i])
					cutline(i, i + gameopt.rowwidth);
			}

			linesbeingcut = linesfalse;
			newrandompiece();
		}
	}
	bool checklineandnewpiece = false, callgameover = false;

	nextpiece_rot += gameopt.physicstep;
	nextpiece_rot = fmod(nextpiece_rot, 2*M_PI);

	physichandler.step(level, [&](float x, float y)
	{
		if (gamestate == GAMEOVER)
			return;
		// The falling piece has landed:
		// time to generate another piece if the screen is not full
		physichandler.untagfallingpiece();

		if (y > 0)
			checklineandnewpiece = true;
		else
			callgameover = true;
	});
	if (checklineandnewpiece && gameopt.gametype == GameOptions::CUTTING)
	{
		float totalcuttedarea = 0.0;
		int cuttedlines = 0;
		updatelinearea();
		for (float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
		{
			if (linearea[i] > gameopt.cuttingrowarea)
			{
				// Cutting should happened
				totalcuttedarea = linearea[i];
				cuttedlines++;
				linesbeingcut[i] = true;
			}
		}
		if (cuttedlines != 0)
		{
			// Update score
			int scoreadd = ceil(pow((cuttedlines*3),pow((totalcuttedarea/(10*cuttedlines)),10))*20+cuttedlines*cuttedlines*40);
			score += scoreadd;
			linesortiles += cuttedlines;
			level = linesortiles/10;

			updatescoregraphic();

			// Set all pieces velocity to 0
			physichandler.iteratepieces([&](PhysicPiece * php){
				php->standstill();
			});

			gamestate = CUTPAUSED;
			//TODO: make this parametric
			cutpausecontdown = 60;

			// Return!! Avoid creating newpiece
			return;
		}

		newrandompiece();
	}
	else if (checklineandnewpiece && gameopt.gametype == GameOptions::STACK)
	{
		score += 100;
		linesortiles += 1;
		level = 0;

		updatescoregraphic();
		newrandompiece();
	}

	if (callgameover)
	{
		physichandler.gameover();
		gamestate = GAMEOVER;
	}
	if (gamestate == GAMEOVER)
	{
		bool nopiece = true;
		physichandler.iteratepieces([&](PhysicPiece * php){
			nopiece = false;
			//If piece fallen outside of the screen
			if (php->getY() > gameopt.rows + 4 * gameopt.rowwidth )
			{
				GamePiece * pgp = static_cast<GamePiece*>(php->getUserData());
				deletepiece(pgp);
			}
		});
		if (nopiece)
		{
			//END GAME
		}
	}

}

void GameHandler::step_graphic()
{
	if (gameopt.gametype == GameOptions::CUTTING)
	{
		updatebarscompleteness += gameopt.updatebarsfreq;
		if (updatebarscompleteness > 1.0)
		{
			updatebarscompleteness -= 1.0;
			updatelinearea();
		}
	}
	graphichandler.beginrender(texthandler);
	physichandler.iteratepieces([&](PhysicPiece * php){
		graphichandler.renderpiece(php->getX(),php->getY(),php->getRot(),static_cast<GamePiece *>(php->getUserData())->grp);
	});
	if (gamestate != GAMEOVER)
		graphichandler.renderpiece(gameopt.columns + 5.0F, 15.0F, nextpiece_rot, graphicpieces_uncutted[nextpiece_type].get());
	if ((gamestate == CUTPAUSED || gamestate == CUTPAUSED_PAUSED) && (cutpausecontdown%30) > 15 )
		graphichandler.endrender(linecompleteness, linesbeingcut);
	else
		graphichandler.endrender(linecompleteness, linesfalse);
}

bool GameHandler::step_logic()
{
	bool continuerunning = true;

	inputhandler.process_input(
		[&]()//EXIT
		{
			continuerunning = false;
		},
		[&]()//LEFT
		{
			if(gamestate != RUNNING)
				return;
			physichandler.piecemove(-1);
		},
		[&]()//RIGHT
		{
			if(gamestate != RUNNING)
				return;
			physichandler.piecemove(1);

		},
		[&]()//DOWN
		{
			if(gamestate != RUNNING)
				return;
			physichandler.pieceaccelerate();
		},
		[&]()//Z
		{
			if(gamestate != RUNNING)
				return;
			physichandler.piecerotate(-1);
		},
		[&]()//X
		{
			if(gamestate != RUNNING)
				return;
			physichandler.piecerotate(1);
		},
		[&]()//ENTER PRESS
		{
			togglepause();
		}
	);
	return continuerunning;
}
