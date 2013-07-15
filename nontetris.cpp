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

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include "timemanager.h"
#endif


using namespace std;

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

	#ifdef EMSCRIPTEN
	void newrandompiece(PhysicHandler & phh, GraphicHandler & grh)
	{
		auto & p = pieces[rand()%pieces.size()];
		phh.createpiece(p, 5.125, -1, 0.0, grh.createpiece(p));
	}

	#else
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, pieces.size()-1);

	void newrandompiece(PhysicHandler & phh, GraphicHandler & grh)
	{
		auto & p = pieces[dis(gen)];
		phh.createpiece(p, 5.125, -1, 0.0, grh.createpiece(p));
	}
	#endif
}

bool one_iteration(PhysicHandler & phh, GraphicHandler & grh)
{
	bool running = true;
	//TODO: restructure main loop so that eventually multiple physic steps are done at once
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
	return running;
}
#ifdef EMSCRIPTEN
PhysicHandler * pphh;
GraphicHandler * pgrh;

void oneiterationwrapper()
{
	one_iteration(*pphh,*pgrh);
}

#endif

int main(int argc, char * argv[])
{
	PhysicHandler phh(10.25, 18);
	GraphicHandler grh(600,540);

	newrandompiece(phh, grh);

	#ifdef EMSCRIPTEN
	pphh = &phh;
	pgrh = &grh;
	emscripten_set_main_loop( oneiterationwrapper, 0, false);
	#else
	bool running = true;
	MyTime next;
	while (running)
	{
		running = one_iteration(phh, grh);
		next+=(MyTime(0,1000000000.0/60.0));
		next.sleepuntil();
	}
	#endif
	return EXIT_SUCCESS;
}
