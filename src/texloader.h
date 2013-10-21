#ifndef _TEXLOADER_H
#define _TEXLOADER_H

#include <functional>
#include <array>

class TextureLoader
{
	int textodo;
	std::function<void()> texdonefunc;

public:
	template<typename T>
	void load(T v, const std::function<void()> & f)
	{
		texdonefunc = f;
		textodo = 0;
		for(const auto & s: v)
		{
			auto *imgel = static_cast<client::HTMLImageElement*>(client::document.createElement("img"));
			imgel->addEventListener("load",client::Callback([&](){
				if ((--textodo)==0)
					texdonefunc();
			}));
			imgel->setAttribute("style", "display:none");
			imgel->setAttribute("id", s);
			imgel->setAttribute("src",s);
			auto * elem = static_cast<client::Node*>(client::document.getElementById("texcontainer"));
			elem->appendChild(static_cast<client::Element*>(imgel));
			textodo++;
		}
	}
};
#endif //_TEXLOADER_H
