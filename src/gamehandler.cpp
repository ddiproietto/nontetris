#include "gamehandler.h"

#include "myutil.h"

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

GameHandler::GameHandler(const GraphicOptions & gopt, const FileLoader & fl, float width, float height)
{
	phphysic = new PhysicHandler (width, height);
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

bool GameHandler::oneiteration()
{
	PhysicHandler &phh = *phphysic;
	GraphicHandler &grh = *phgraphic;
	InputHandler &inh = *phinput;

	bool running = true;

	//TODO: restructure main loop so that eventually multiple physic steps are done at once
	phh.step([&](float x, float y)
	{
		//The falling piece has landed:
		//time to generate another piece if the screen is not full
		if( y > 0 )
			newrandompiece();
	});

	grh.render([this](const std::function<void(float x, float y, float rot, GraphicPiece * d)> & x)
	{
		for(auto i : ingamepieces)
		{
			auto php = i.php;
			auto grp = i.grp;
			x(php->getX(),php->getY(),php->getRot(),grp);
		}
			
	});

	inh.process_input([&]()//EXIT
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
