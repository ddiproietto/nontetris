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
