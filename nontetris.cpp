#include <cstdlib>
#include <iostream>
#include <array>

#include "NontetrisConfig.h"

#include "polygon.h"
#include "piece.h"
#include "physichandler.h"
#include "graphichandler.h"
#include "inputhandler.h"
#include "myutil.h"
#include "timemanager.h"

using namespace std;

//#define newpiece(p) phh.createpiece(p, 5.125, -1, 0.0, grh.createpiece(p));

namespace
{
	auto pieces = make_array(
		piece <float> ({ { -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5} }), //I
		piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,1}, { 0.5,1}, { 0.5,0}, { -1.5,0} }), //J
		piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,0}, { -0.5,0}, { -0.5,1}, { -1.5,1} }), //L
		piece <float> ({ { -1,-1}, { 1,-1}, { 1,1}, { -1,1} }), //O
		piece <float> ({ { -0.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -1.5,1}, { -1.5,0}, { -0.5,0} }), //S
		piece <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }), //Z
		piece <float> ({ { -1.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }) //T
	);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, pieces.size()-1);

	void newrandompiece(PhysicHandler & phh, GraphicHandler & grh)
	{
		auto & p = pieces[dis(gen)];
		phh.createpiece(p, 5.125, -1, 0.0, grh.createpiece(p));
	}
}

int main(int argc, char * argv[])
{
	PhysicHandler phh(10.25, 18);
	GraphicHandler grh(308,540);

	newrandompiece(phh, grh);

	bool running = true;
	MyTime next;
	while (running)
	{
		//TODO: restructure main loop so that eventually multiple physic steps are done at once
		//TODO: restructure timing
		phh.step([&](float x, float y)
		{
			//FALLING PIECE HAS LANDED
			if( y > 0 )
				newrandompiece(phh, grh);
		});
		//phh.debugprint();
		//phh.drawbodies([](float x, float y, float rot, void * d){cout <<"(" << x << ", " << y << "); rot:"<< rot << endl;});
		//running = grh.render(std::bind(&PhysicHandler::drawbodies, &phh, std::placeholders::_1));
		grh.render([& phh](std::function<void(float x, float y, float rot, void * d)> x){phh.drawbodies(x);}); //also sleeps because of vsync(not on every platform)
		process_input([&]()//EXIT
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
		next+=(MyTime(0,1000000000.0/60.0));
		next.sleepuntil();
	}
	return EXIT_SUCCESS;
}
