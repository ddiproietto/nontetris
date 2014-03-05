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

#include "myutil.h"

#define BOOST_RESULT_OF_USE_DECLTYPE
#include <boost/range/adaptor/transformed.hpp>

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

GameHandler::GameHandler(const GraphicOptions & gopt, const GameOptions & gameopt, const FileLoader & fl, double physicstep)
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

	auto * php = phphysic->createpiece(p, 5.125, -1, 0.0, NULL);
	auto * grp = phgraphic->createpiece(p);

	ingamepieces.insert(ingamepieces.begin(), GamePiece{.php=php,.grp=grp});
}

void GameHandler::step_physic()
{
	PhysicHandler &phh = *phphysic;

	phh.step([&](float x, float y)
	{
		//The falling piece has landed:
		//time to generate another piece if the screen is not full
		if( y > 0 )
			newrandompiece();
	});
}

void GameHandler::step_graphic()
{
	GraphicHandler &grh = *phgraphic;
	PhysicHandler &phh = *phphysic;

	grh.render(ingamepieces | boost::adaptors::transformed([](const struct GamePiece & i)->std::tuple<float, float, float, GraphicPiece *>{
				auto php = i.php;
				auto grp = i.grp;
				return std::make_tuple(php->getX(), php->getY(), php->getRot(), grp);
			}));
#if 0
	grh.beginrender();
	for(auto i : ingamepieces)
	{
		auto php = i.php;
		auto grp = i.grp;
		grh.renderpiece(php->getX(),php->getY(),php->getRot(),grp);
	}
	grh.endrender();
#endif
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
