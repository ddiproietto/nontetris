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
#include "myutil.h"

#include "emscripten.h"

using namespace std;

namespace
{
	const double PHYSICSTEP = 1.0/60.0;
	FileLoader fileloader;
	GameHandler * pgh;

	void oneiterationwrapper()
	{
		pgh->step_physic();
		pgh->step_logic();
		pgh->step_graphic();
	}
}

int main(int argc, char * argv[])
{
	GraphicOptions gopt =
	{
		.width      = 600,
		.height     = 540,
		.fullscreen = false,
	};
	GameOptions gameopt =
	{
		.rows = 18,
		.columns = 10.25,
	};
	pgh = new GameHandler (gopt, gameopt, fileloader, PHYSICSTEP);
	emscripten_set_main_loop(oneiterationwrapper, 60, 1);

	return EXIT_SUCCESS;
}
