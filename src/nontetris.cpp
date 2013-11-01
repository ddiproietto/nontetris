#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#include "texloader.h"
#endif
#include <cstdlib>
#include <array>
#include <list>

#include "NontetrisConfig.h"

#include "polygon.h"
#include "piece.h"
#include "graphichandler.h"
#include "physichandler.h"
#include "inputhandler.h"

#include "fileloader.h"
#include "myutil.h"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#elif defined(__DUETTO__)
//Bridge method implemented in JS since feature testing is not yet supported by Duetto
extern "C" {
	void compatRequestAnimationFrame(const client::EventListener&);
}
#else
//#include "timemanager.h"
#include <chrono>
#include <thread>
#ifdef __MINGW32__
//Mingw doesn't have std:::this_thread
#include <windows.h> //For Sleep
#endif
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
	struct GamePiece
	{
		PhysicPiece * php;
		GraphicPiece * grp;
	};
	list<GamePiece> ingamepieces;

	#if defined(EMSCRIPTEN) || defined(__DUETTO__)
	void newrandompiece(PhysicHandler & phh, GraphicHandler & grh)
	{
		#ifdef __DUETTO__
		auto & p = pieces[int(client::Math.random()*pieces.size())];
		#else
		auto & p = pieces[rand()%pieces.size()];
		#endif
		auto * php = phh.createpiece(p, 5.125, -1, 0.0, NULL);
		auto * grp = grh.createpiece(p);
		ingamepieces.insert(ingamepieces.begin(), GamePiece{.php=php,.grp=grp});
	}
	#else
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, pieces.size()-1);

	void newrandompiece(PhysicHandler & phh, GraphicHandler & grh)
	{
		auto & p = pieces[dis(gen)];
		auto * php = phh.createpiece(p, 5.125, -1, 0.0, NULL);
		auto * grp = grh.createpiece(p);
		ingamepieces.insert(ingamepieces.begin(), GamePiece{.php=php,.grp=grp});
	}
	#endif
}

bool one_iteration(PhysicHandler & phh, GraphicHandler & grh, InputHandler & inh)
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
	//grh.render([& phh](std::function<void(float x, float y, float rot, void * d)> x){phh.drawbodies(x);}); //also sleeps because of vsync(not on every platform)
	grh.render([](const std::function<void(float x, float y, float rot, GraphicPiece * d)> & x)
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
PhysicHandler * pphh = NULL;
GraphicHandler * pgrh = NULL;
InputHandler * pinh = NULL;
FileLoader * fileloader = NULL;

#if defined(EMSCRIPTEN) || defined(__DUETTO__)

void oneiterationwrapper()
{
	one_iteration(*pphh, *pgrh, *pinh);

	#if defined(__DUETTO__)
	compatRequestAnimationFrame(client::Callback(oneiterationwrapper));
	#endif
}
#endif
#ifdef __DUETTO__

TextureLoader * texloader = NULL;

char argv1 [] = "nontetris";
char * argv [] = {argv1};
int main(int argc, char * argv[]);

auto filestoload = make_array(
	"shader.frag",
	"shader.vert",
	"shaderident.vert"
);
auto texturestoload = make_array(
	"imgs/newgamebackground.png",
	"imgs/pieces/1.png",
	"imgs/pieces/2.png",
	"imgs/pieces/3.png",
	"imgs/pieces/4.png",
	"imgs/pieces/5.png",
	"imgs/pieces/6.png",
	"imgs/pieces/7.png"
);

int webMain() [[client]]
{
	texloader = new TextureLoader;
	fileloader = new FileLoader;

	client::document.addEventListener("DOMContentLoaded", client::Callback([=](){
		//The DOM has been loaded
		client::console.log("DOMloaded");
		fileloader->load(filestoload, [=](){
			client::console.log("FILESloaded");
			//The files have been loaded
			texloader->load(texturestoload, [=](){
				client::console.log("TEXTURESloaded");
				//The textures have been loaded
				main(1, argv);
			});
		});
	}));

	return 0;
}
#endif

int main(int argc, char * argv[])
{
	pphh = new PhysicHandler (10.25, 18);
	pgrh = new GraphicHandler (600, 540, fileloader);
	#if !defined( __DUETTO__) && (GLFW_VERSION_MAJOR == 3)
	pinh = new InputHandler(pgrh->getglfwwindow());
	#else
	pinh = new InputHandler();
	#endif

	PhysicHandler &phh = *pphh;
	GraphicHandler &grh = *pgrh;
	InputHandler &inh = *pinh;

	newrandompiece(phh, grh);

	#ifdef EMSCRIPTEN
	emscripten_set_main_loop(oneiterationwrapper, 0, false);
	#elif defined(__DUETTO__)
	compatRequestAnimationFrame(client::Callback(oneiterationwrapper));
	#else
	bool running = true;
	auto next = std::chrono::steady_clock::now();
	while (running)
	{
		running = one_iteration(phh, grh, inh);
		next+=(std::chrono::nanoseconds(1000000000/60));
		#ifdef __MINGW32__
		//Mingw doesn't have std::this_thread
		auto diff = next - std::chrono::steady_clock::now();
		int millitosleep = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		if(millitosleep > 0)
			Sleep(millitosleep);
		#else
		std::this_thread::sleep_until(next);
		#endif
	}
	delete pinh;
	delete pgrh;
	delete pphh;
	#endif
	return EXIT_SUCCESS;
}
