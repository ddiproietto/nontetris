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
	TextureLoader texloader;
	FileLoader fileloader;
	GameHandler * pgh;

	void oneiterationwrapper()
	{
		pgh->oneiteration();

		compatRequestAnimationFrame(client::Callback(oneiterationwrapper));
	}

	void allloaded(const FileLoader & fl)
	{

		GraphicOptions gopt =
		{
			.width      = 600,
			.height     = 540,
			.fullscreen = false,
		};
		pgh = new GameHandler(gopt, fileloader, 10.25, 18);
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

	//TODO: the two operation could be done in parallel
	
	client::document.addEventListener("DOMContentLoaded", client::Callback([=](){
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
	}));

	return 0;
}
