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
#ifndef _CUTTER_H
#define _CUTTER_H

#include <vector>
#include <algorithm>
#include <array>

#include "polygon.h"

template <typename T>
class Cutter
{
	enum PositionState {UP, MID, DOWN};
	T up, down;

	PositionState ytoposstate(T y);
public:
	Cutter(T _up, T _down);
	template <typename C1, typename C2, typename C3>
	bool cutbodyheight(const polygon<T> & p, C1 & upres, C2 & downres, C3 & midres);
};

template <typename T>
Cutter<T>::Cutter(T _up, T _down):up(_up),down(_down)
{
}

template <typename T>
typename Cutter<T>::PositionState Cutter<T>::ytoposstate(T y)
{
	if (y > down)
		return DOWN;
	else if (y < up)
		return UP;
	else
		return MID;
}

template <typename T>
point<T> intersection(const point<T> & p1, const point<T> & p2, T y)
{
	point <T> ret;
	ret.y = y;
	ret.x = ((p2.x-p1.x)/(p2.y-p1.y))*(y-p1.y) + p1.x;
	return ret;
}


template <typename T>
void myslice_internal(const polygon<T> & orig, polygon<T> & dest)
{

}

template <typename T, typename... Params>
void myslice_internal(const polygon<T> & orig, polygon<T> & dest, int from, int to, Params... params)
{
	if (from > to)
		to += orig.size();

	for (int i = from; i <= to; ++i)
	{
		dest.push_back(orig[i]);
	}
	myslice_internal(orig, dest, params...);
}

/* This returns a polygon that contains selected intervals of
 * vertices from orig. The code is so long because
 * - there can be as many intervals as one would like
 * - polygon is a circular vector
 * TODO: maybe this function should be a member of polygon<T>
 */
template <typename T, typename... Params>
polygon<T> myslice(const polygon<T> & orig, Params... params)
{
	polygon<T> ret;
	myslice_internal(orig, ret, params...);
	return ret;
}

/* This function takes a circular array and determines if
 * it is sorted ascendingly
 *
 * If arr is sorted ascendingly the function returns TRUE
 * If arr is sorted descendingly or not sorted at all the function returns FALSE
 */
template<typename T>
static int isvec4ordasc(T arr)
{
	int notascendingsteps = (arr[0]>arr[1]) + (arr[1]>arr[2]) + (arr[2] > arr[3]) + (arr[3] > arr[0]);

	return notascendingsteps <= 1;
}



/* Returns true if a part of the polygon is between the lines.
 * Otherwise it does not put it in upres or downres */
template <typename T> template<typename C1, typename C2, typename C3>
bool Cutter<T>::cutbodyheight(const polygon<T> & p, C1 & upres, C2 & downres, C3 & midres)
{
	PositionState actstat, prevstat;
	polygon<T> newp;
	const point<T> * pprev_vertex = &(p.back());

	std::vector<int> up_intersections;
	std::vector<int> down_intersections;

	prevstat = ytoposstate(pprev_vertex->y);

	/* Creates a new polygon newp. It has also intersections with up and down
	 * as vertices. The indexes of the intersections with up and down are stored
	 * in up_intersections and down_intersections */
	for (const auto & vertex : p)
	{
		const point<T> & prev_vertex = *pprev_vertex;


		actstat = ytoposstate(vertex.y);

		if ((actstat == MID && prevstat == DOWN) ||
			(actstat == DOWN && prevstat == MID))
		{
			newp.push_back(intersection(prev_vertex, vertex, down));
			down_intersections.push_back(newp.size()-1);
		}
		else if ((actstat == MID && prevstat == UP) ||
			(actstat == UP && prevstat == MID))
		{
			newp.push_back(intersection(prev_vertex, vertex, up));
			up_intersections.push_back(newp.size()-1);
		}
		else if (actstat == UP && prevstat == DOWN)
		{
			newp.push_back(intersection(prev_vertex, vertex, down));
			down_intersections.push_back(newp.size()-1);
			newp.push_back(intersection(prev_vertex, vertex, up));
			up_intersections.push_back(newp.size()-1);
		}
		else if (actstat == DOWN && prevstat == UP)
		{
			newp.push_back(intersection(prev_vertex, vertex, up));
			up_intersections.push_back(newp.size()-1);
			newp.push_back(intersection(prev_vertex, vertex, down));
			down_intersections.push_back(newp.size()-1);
		}

		newp.push_back(vertex);

		prevstat = actstat;
		pprev_vertex = &vertex;
	}

	auto sortcriterion = [&](const int & a, const int & b)->bool
	{
		return newp[a].x < newp[b].x;
	};
	
	/* Sort intersections by the x coordinate */
	sort(up_intersections.begin(), up_intersections.end(), sortcriterion);
	sort(down_intersections.begin(), down_intersections.end(), sortcriterion);

	/* Split polygon by intersections */

	if (up_intersections.size() == 0 && down_intersections.size() == 0)
	{
		/* No intersections. The polygon must be entirely above, below,
		 * or in the middle of the lines */
		if (newp[0].y > down)
		{
			downres.push_back(newp);
			return false;
		}
		else if (newp[0].y < up)
		{
			upres.push_back(newp);
			return false;
		}
		else
		{
			midres.push_back(newp);
			return true;
		}
	}
	else if (up_intersections.size() == 0 && down_intersections.size() == 2)
	{
		downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
		midres.push_back(myslice(newp, down_intersections[0], down_intersections[1]));
	}
	else if (up_intersections.size() == 2 && down_intersections.size() == 0)
	{
		upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
		midres.push_back(myslice(newp, up_intersections[1], up_intersections[0]));
	}
	else if (up_intersections.size() == 2 && down_intersections.size() == 2)
	{
		upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
		downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
		midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
		/*
		std::cout<<newp<<std::endl;
		std::cout<<"upint:"<<up_intersections[0] << "," <<up_intersections[1]<<std::endl;
		std::cout<<"downint:"<<down_intersections[0] << "," <<down_intersections[1]<<std::endl;
		*/
	}
	else if (up_intersections.size() == 0 && down_intersections.size() == 4)
	{
		if(isvec4ordasc(down_intersections))
		{
			midres.push_back(myslice(newp, down_intersections[0], down_intersections[1]));
			midres.push_back(myslice(newp, down_intersections[2], down_intersections[3]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[2], down_intersections[3], down_intersections[0]));
		}
		else
		{
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			midres.push_back(myslice(newp, down_intersections[0], down_intersections[3], down_intersections[2], down_intersections[1]));
		}
	}
	else if (up_intersections.size() == 4 && down_intersections.size() == 0)
	{
		if(isvec4ordasc(up_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, up_intersections[1], up_intersections[2], up_intersections[3], up_intersections[0]));
		}
		else
		{
			midres.push_back(myslice(newp, up_intersections[3], up_intersections[2]));
			midres.push_back(myslice(newp, up_intersections[1], up_intersections[0]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
		}
	}
	else if (up_intersections.size() == 2 && down_intersections.size() == 4)
	{
		upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
		if(isvec4ordasc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[2], down_intersections[3], down_intersections[0]));

			if(isvec4ordasc(make_array(down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1])))
			{
				midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
				midres.push_back(myslice(newp, down_intersections[2], down_intersections[3]));
			}
			else
			{
				midres.push_back(myslice(newp, down_intersections[2], up_intersections[0], up_intersections[1], down_intersections[3]));
				midres.push_back(myslice(newp, down_intersections[0], down_intersections[1]));
			}
		}
		else
		{
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[3], down_intersections[2], down_intersections[1]));
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
		}
	}
	else if (up_intersections.size() == 4 && down_intersections.size() == 2)
	{
		downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
		if(isvec4ordasc(up_intersections))
		{
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
		}
		else
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			if(isvec4ordasc(make_array(up_intersections[1],down_intersections[1], down_intersections[0], up_intersections[0])))
			{
				midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
				midres.push_back(myslice(newp, up_intersections[3], up_intersections[2]));
			}
			else
			{
				midres.push_back(myslice(newp, up_intersections[3], down_intersections[1], down_intersections[0], up_intersections[2]));
				midres.push_back(myslice(newp, up_intersections[1], up_intersections[0]));
			}
		}
	}
	else if (up_intersections.size() == 4 && down_intersections.size() == 4)
	{
		if(!isvec4ordasc(up_intersections) && !isvec4ordasc(down_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
			midres.push_back(myslice(newp, down_intersections[2], up_intersections[2], up_intersections[3], down_intersections[3]));
		}
		else if(isvec4ordasc(up_intersections) && !isvec4ordasc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[3], down_intersections[2], down_intersections[1]));
		}
		else if(isvec4ordasc(up_intersections) && isvec4ordasc(down_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
			midres.push_back(myslice(newp, down_intersections[2], up_intersections[2], up_intersections[3], down_intersections[3]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[2], down_intersections[3], down_intersections[0]));
		}
		else
		{
			//TODO: error strange ordering
		}
	}
	else
	{
		//TODO: error strange number of intersections
	}

return true;
}

#endif //_CUTTER_H
