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
