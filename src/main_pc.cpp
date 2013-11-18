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

#ifdef __MINGW32__
//Mingw doesn't have std:::this_thread
#include <windows.h> //For Sleep
#endif

using namespace std;

int main(int argc, char * argv[])
{
	GraphicOptions gopt =
	{
		.width      = 600,
		.height     = 540,
		.fullscreen = false,
	};
	FileLoader fl;
	GameHandler gh(gopt, fl, 10.25, 18);

	bool running = true;
	auto next = std::chrono::steady_clock::now();
	while (running)
	{
		running = gh.oneiteration();
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
	return EXIT_SUCCESS;
}
