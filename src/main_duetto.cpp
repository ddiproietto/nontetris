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

/* Since feature testing (for functions) is not possible in C++,
 * this function is defined. It is then set directly in JS,
 * using __asm__, in webMain
 */

extern "C" {
	void compatRequestAnimationFrame(const client::EventListener&);
}

namespace
{
	bool running = true;
	int timerHandler;

	TextureLoader texloader;
	FileLoader fileloader;
	GameHandler * pgh;

	void oneiterationwrappergraphic()
	{
		if(!running)
			return;

		pgh->step_graphic();
		compatRequestAnimationFrame(client::Callback(oneiterationwrappergraphic));
	}

	void oneiterationwrapperlogic()
	{
		pgh->step_physic();
		running = pgh->step_logic();

		if(!running) {
			//SHUTDOWN
			delete pgh;
			client::clearInterval(timerHandler);
		}
	}

	void allloaded(const FileLoader & fl)
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
		pgh = new GameHandler(gameopt, fileloader);

		oneiterationwrappergraphic();
		timerHandler = client::setInterval(client::Callback(oneiterationwrapperlogic), gameopt.physicstep*1000);
	}
}

int webMain() [[client]]
{
	auto filestoload = make_array(
		"shader.frag",
		"shader.vert",
		"shadercomp.frag"
	);
	auto texturestoload = make_array(
		"imgs/newgamebackground.png",
		"imgs/newgamebackgroundgamea.png",
		"imgs/font.png",
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

	__asm__(" \
		window._compatRequestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame || \
			window.webkitRequestAnimationFrame || window.msRequestAnimationFrame;");

	//TODO: think of a better way to compare
	if (client::document.get_readyState() != new client::String("loading")) {
		domloaded();
	} else {
		client::document.addEventListener("DOMContentLoaded", client::Callback(domloaded));
	}

	return 0;
}
