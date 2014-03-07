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
#include <duetto/client.h>
#include <duetto/clientlib.h>
#include "texloader.h"

#include "gamehandler.h"

#include "myutil.h"

//Bridge method implemented in JS since feature testing is not yet supported by Duetto
extern "C" {
	void compatRequestAnimationFrame(const client::EventListener&);
}

namespace
{
	const double PHYSICSTEP = 1.0/60.0;

	TextureLoader texloader;
	FileLoader fileloader;
	GameHandler * pgh;

#ifdef MULTIPLE_PHYSICS_STEPS
	double next;
#endif

	void oneiterationwrapper()
	{
		bool running = true;
#ifdef MULTIPLE_PHYSICS_STEPS
		auto * nowdate = new client::Date::Date();
		double now = nowdate->getTime();
		//If lag is too much, maybe it means simply that the user is doing something else
		if(now - next > 2000)
		{
			now = next - 1;
		}
		while(now > next && running)
		{
			pgh->step_physic();
			running = pgh->step_logic();

			next += PHYSICSTEP * 1000.0;
		}
#else
		pgh->step_physic();
		running = pgh->step_logic();

#endif
		pgh->step_graphic();

		if(running) {
			compatRequestAnimationFrame(client::Callback(oneiterationwrapper));
		} else {
			//SHUTDOWN
			delete pgh;
		}

	}

	void allloaded(const FileLoader & fl)
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
		pgh = new GameHandler(gopt, gameopt, fileloader, PHYSICSTEP);


#ifdef MULTIPLE_PHYSICS_STEPS
		auto * nowdate = new client::Date::Date();
		next = nowdate->getTime();
#endif
		oneiterationwrapper();
	}
}

int webMain() [[client]]
{
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

	//TODO: the two operations could be done in parallel

	auto domloaded = [=](){
		//The DOM has been loaded
		client::console.log("DOMloaded");
		fileloader.load(filestoload, [=](){
			client::console.log("FILESloaded");
			//The files have been loaded
			texloader.load(texturestoload, [=](){
				client::console.log("TEXTURESloaded");
				//The textures have been loaded
				allloaded(fileloader);
			});
		});
	};

	//In the future asm could be used
	client::eval(" \
		_compatRequestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame || \
			window.webkitRequestAnimationFrame || window.msRequestAnimationFrame;");

	//TODO: think of a better way to compare
	if (client::document.get_readyState() != new client::String("loading")) {
		domloaded();
	} else {
		client::document.addEventListener("DOMContentLoaded", client::Callback(domloaded));
	}

	return 0;
}
