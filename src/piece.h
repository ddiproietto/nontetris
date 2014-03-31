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
#ifndef _PIECE_H
#define _PIECE_H

#include <vector>

#include "polygon.h"

#include "convexer.h"


template <class T = float>
class piece
{
	polygon<T> shape;
	int type;
	std::vector<polygon<T>> convshapes;
public: 
	piece(const polygon<T> & p, int ptype):shape(p),type(ptype)
	{
		updateconvex();
	}
	piece(std::initializer_list<point<T>> l, int type):piece(polygon<T>(l),type)
	{
	}
	auto begin() const -> decltype(convshapes.cbegin())
	{
		return convshapes.cbegin();
	}
	auto end() const -> decltype(convshapes.cend())
	{
		return convshapes.cend();
	}
	auto size() const -> decltype(convshapes.size())
	{
		return convshapes.size();
	}
	auto totsize() const -> decltype(convshapes.size())
	{
		int s = 0;
		for (auto i: convshapes)
		{
			s += i.size();
		}
		return s;
	}
	polygon<T> & operator[](size_t n)
	{
		return convshapes[n];
	}
	int getType() const 
	{
		return type;
	}

	void updateconvex()
	{
		convshapes = convexer(shape);
	}
	polygon<T> getshape() const
	{
		return shape;
	}
};

#endif //_PIECE_H
