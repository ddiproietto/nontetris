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
#include <cstdlib>
#include <array>
#include <list>
#include <chrono>
#include <thread>

#include "NontetrisConfig.h"

#include "polygon.h"
#include "piece.h"

#include "gamehandler.h"

#include "fileloader.h"
#include "texloader.h"
#include "myutil.h"

#include "emscripten.h"

using namespace std;

namespace
{
	FileLoader fileloader;
	TextureLoader texloader;
	GameHandler * pgh;

	void oneiterationwrapper()
	{
		bool running;
		pgh->step_physic();
		running = pgh->step_logic();
		pgh->step_graphic();

		if(!running)
		{
			//SHUTDOWN
			delete pgh;
			emscripten_cancel_main_loop();
		}
	}
}

int main(int argc, char * argv[])
{
	const GameOptions gameopt =
	{
		.gametype = GameOptions::CUTTING,

		.rows     = 18.0,
		.columns  = 10.25,
		.rowwidth = 1.0,

		.cuttingrowarea = 8.0,
		.updatebarsfreq = 0.2,

		.physicstep = 1.0/60.0,

		.width      = 600,
		.height     = 540,
		.fullscreen = false,
		.piecesAA   = 4,
	};
	pgh = new GameHandler (gameopt, fileloader, texloader);
	emscripten_set_main_loop(oneiterationwrapper, 0, 0);

	return EXIT_SUCCESS;
}
