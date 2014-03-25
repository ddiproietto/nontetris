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

#include "myutil.h"

#include "cutter.h"

namespace
{
	auto pieces = make_array(
			piece <float> ({ { -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5} }, 0), //I
			piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,1}, { 0.5,1}, { 0.5,0}, { -1.5,0} }, 1), //J
			piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,0}, { -0.5,0}, { -0.5,1}, { -1.5,1} }, 2), //L
			piece <float> ({ { -1,-1}, { 1,-1}, { 1,1}, { -1,1} }, 3), //O
			piece <float> ({ { -0.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -1.5,1}, { -1.5,0}, { -0.5,0} }, 4), //S
			piece <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }, 5), //Z
			piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }, 6) //T
			);
}

GameHandler::GameHandler(const GraphicOptions & gopt, const GameOptions & _gameopt, const FileLoader & fl, double physicstep):gameopt(_gameopt)
{
	phphysic = new PhysicHandler (gameopt.columns, gameopt.rows, physicstep);
	phgraphic = new GraphicHandler (gopt, fl);
	phinput = new InputHandler(phgraphic->toinput());

	//Start the game with a new random piece!
	newrandompiece();
}

GameHandler::~GameHandler()
{
	delete phphysic;
	delete phgraphic;
	delete phinput;
}

void GameHandler::newpiece(const piece<float> & p, float x, float y, float rot)
{
	//TODO: free this memory at exit
	auto * gamepiece = new GamePiece(p);
	gamepiece->php = phphysic->createpiece(p, x, y, rot, gamepiece);
	gamepiece->grp = phgraphic->createpiece(p);
}

void GameHandler::newrandompiece()
{
//Different ways of generating random numbers
#if defined (__DUETTO__)
	int randpieceindex = int(client::Math.random()*pieces.size());
#elif defined (EMSCRIPTEN)
	int randpieceindex = rand()%pieces.size();
#else
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, pieces.size()-1);

	int randpieceindex = dis(gen);
#endif

	auto & p = pieces[randpieceindex];
	newpiece(p, gameopt.columns/2, -1, 0.0);
}

void GameHandler::cutlineeventually(float from, float to)
{
	float x0 = 0;
	float y0 = from;
	float x1 = gameopt.columns;
	float y1 = to;

	Cutter<float> cutter(y0, y1);

	float linearea = 0;

	struct DeletePiece
	{
		std::list<polygon<float>> remainders;
		GamePiece * pgp;
		int originaltype;
		float x,y,rot;
	};
	std::list<DeletePiece> deletelist;
	std::list<polygon<float>> midremainders;

	phphysic->getpieces_in_rect(x0, y0, x1, y1, [y0, y1, &cutter, &linearea, &deletelist, &midremainders](PhysicPiece * php){
		polygon<float> p (static_cast<GamePiece *>(php->getUserData())->p.getshape());
		struct DeletePiece dp;

		//Transform the piece coordinates from local to global
		for (auto & vertex: p)
		{
			vertex.rotate(php->getRot());
			vertex.translate(php->getX(), php->getY());
		}

		bool isinbetween = cutter.cutbodyheight(p, dp.remainders, dp.remainders, midremainders);

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

	for (const auto & midp: midremainders)
	{
		linearea += midp.area();
	}
	
	if (linearea > gameopt.cuttingrowarea)
	{
		for(auto & dp: deletelist)
		{
			GamePiece * pgp = dp.pgp;
			phgraphic->deletepiece(pgp->grp);
			phphysic->destroypiece(pgp->php);
			delete pgp;
			
			//Create remainders
			for(auto & pol: dp.remainders)
			{
				//Transform the piece coordinates from global to local again
				for (auto & vertex: pol)
				{
					vertex.translate(-dp.x, -dp.y);
					vertex.rotate(-dp.rot);
				}
				newpiece(piece<float>(pol, dp.originaltype), dp.x, dp.y, dp.rot);
			}
		}
	}
}

void GameHandler::step_physic()
{
	PhysicHandler &phh = *phphysic;
	bool checklineandnewpiece = false;

	phh.step([&](float x, float y)
	{
		//The falling piece has landed:
		//time to generate another piece if the screen is not full
		if( y > 0 )
			checklineandnewpiece = true;
	});
	if (checklineandnewpiece)
	{
		for(float i = 0.0; i < gameopt.rows; i += gameopt.rowwidth)
			cutlineeventually(i, i + gameopt.rowwidth);
		newrandompiece();
	}
}

void GameHandler::step_graphic()
{
	GraphicHandler &grh = *phgraphic;
	PhysicHandler &phh = *phphysic;

	grh.beginrender();
	phh.drawbodies([&](PhysicPiece * php){
		grh.renderpiece(php->getX(),php->getY(),php->getRot(),static_cast<GamePiece *>(php->getUserData())->grp);
	});
	/*
	for(auto i : ingamepieces)
	{
		auto php = i.php;
		auto grp = i.grp;
		grh.renderpiece(php->getX(),php->getY(),php->getRot(),grp);
	}
	*/
	grh.endrender();
}

bool GameHandler::step_logic()
{
	PhysicHandler &phh = *phphysic;
	InputHandler &inh = *phinput;

	bool running = true;

	inh.process_input(
		[&]()//EXIT
		{
			running = false;
		},
		[&]()//LEFT
		{
			phh.piecemove(-1);
		},
		[&]()//RIGHT
		{
			phh.piecemove(1);

		},
		[&]()//DOWN
		{
			phh.pieceaccelerate();
		},
		[&]()//Z
		{
			phh.piecerotate(-1);
		},
		[&]()//X
		{
			phh.piecerotate(1);
		}
	);
	return running;
}
