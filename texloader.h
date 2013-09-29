#ifndef _TEXLOADER_H
#define _TEXLOADER_H

#include <functional>
#include <array>

int textodo = 0;
std::function<void()> texdonefunc;

void onetexloaded()
{
	//client::console.log("HERE");
	if ((--textodo)==0)
		texdonefunc();

}

template<typename T>
void loadtextures(std::function<void()> f, T v)
{
	texdonefunc = f;
	for(const auto & s: v)
	{
		auto *imgel = static_cast<client::HTMLImageElement*>(client::document.createElement("img"));
		//imgel->set_onload(client::Callback(onetexloaded));
		imgel->addEventListener("load",client::Callback(onetexloaded));
		imgel->setAttribute("style", "display:none");
		imgel->setAttribute("id", s);
		imgel->setAttribute("src",s);
		auto * elem = static_cast<client::Node*>(client::document.getElementById("texcontainer"));
		elem->appendChild(static_cast<client::Element*>(imgel));
		textodo++;
	}
}

#endif //_TEXLOADER_H
