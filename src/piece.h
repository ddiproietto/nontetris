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
//#include <iostream>

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
		convshapes = convexer(shape);
	}
	piece(std::initializer_list<point<T>> l, int type):piece(polygon<T>(l),type)
	{
	}
	auto begin() -> decltype(convshapes.begin())
	{
		return convshapes.begin();
	}
	auto end() -> decltype(convshapes.end())
	{
		return convshapes.end();
	}
	auto size() -> decltype(convshapes.size()) const
	{
		return convshapes.size();
	}
	auto totsize() -> decltype(convshapes.size()) const
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
};

#endif //_PIECE_H