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

#ifdef __CHEERP__
#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#else
#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_CPP
#include "lodepng.h"
#endif

#include <functional>
#include <string>
#include <map>

#include <type_traits>

/* TextureLoader
 *
 * The class is used to load images. It returns HTMLImageElement * if used 
 * with cheerp, unsigned char * (binary data) otherwise. The interfaces are
 * different to be as fast as possible in both platforms.
 *
 * The load() method takes a list of files and a callback function. It is
 * necessary only for cheerp.
 *
 * The get() method later returns the image data. If not using cheerp, the
 * data should be freed with a call to free().
 *
 */

class TextureLoader
{
#ifdef __CHEERP__
	int textodo;
	std::map<std::string, client::HTMLImageElement *> texturesmap;
#endif
	std::function<void()> texdonefunc;

public:
	template<typename T>
	void load(T v, const std::function<void()> &f)
	{
		texdonefunc = f;
#ifdef __CHEERP__
		textodo = 0;
		for(const auto &s: v) {
			auto *imgel = static_cast<client::HTMLImageElement *>(client::document.createElement("img"));
			imgel->addEventListener("load",cheerp::Callback([&](){
				if ((--textodo) == 0) {
					texdonefunc();
				}
			}));
			imgel->setAttribute("src", s);
			texturesmap.insert(make_pair(std::string(s), imgel));

			textodo++;
		}
#else
		texdonefunc();
#endif
	}
#ifdef __CHEERP__
	client::HTMLImageElement *get(const std::string &s) const
	{
		/* Old version of cheerp have a bug with map::find() const */
		auto & tm = const_cast<std::remove_const<decltype(texturesmap)>::type &>(texturesmap);
		auto it = tm.find(s);

		if (it == tm.end()) {
			return NULL;
		}

		return it->second;
	}
#else
	unsigned char *get(const std::string &s, unsigned int *twidth, unsigned int *theight) const
	{
		unsigned char *image;
		lodepng_decode24_file(&image, twidth, theight, s.c_str());
		return image;
	}
#endif
};
#endif /* _TEXLOADER_H */
