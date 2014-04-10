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
#ifndef _TEXTHANDLER_H
#define _TEXTHANDLER_H

#include "glwrapper.h"

#include <string>
#include <list>
#include <vector>

enum class TextAlign {ALIGN_LEFT, ALIGN_RIGHT};

struct GraphicText;

class TextHandler
{
	struct TextFragment
	{
		std::string text;
		TextAlign align;
		float x,y;
		std::vector<float> vbocache;
	};
	std::list<TextFragment> textfragments;
	std::vector<float> vboglobalcache;
	std::vector<int> vboglobalcache_numvertices;
	GLuint vbo;

	friend struct GraphicText;

	bool dirty;
public:
	TextHandler():vbo(0),dirty(true)
	{}
	GraphicText createtextfragment(const std::string & text, TextAlign align, float x, float y);
	void deletetextfragment(GraphicText);

	GLuint getvbo();
	std::vector<float> & getvbo_vector();
	std::vector<int> & getvbo_numvertices();
	void updatevbo();
};

struct GraphicText
{
	decltype(TextHandler::textfragments.end()) fragiterator;
};


#endif // _TEXTHANDLER_H
