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
			imgel->addEventListener("load",duetto::Callback([&](){
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
