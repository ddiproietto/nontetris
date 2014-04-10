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

enum PositionState {UP = 1, MIDUP = (1|2), MID = 2, MIDDOWN = (2|4), DOWN = 4};

template <typename T>
PositionState ytoposstate(T y, T up, T down, T tolerance)
{
	if (y > down+tolerance)
		return DOWN;
	else if (y > down-tolerance)
		return MIDDOWN;
	else if (y > up+tolerance)
		return MID;
	else if (y > up-tolerance)
		return MIDUP;
	else
		return UP;
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

/* Removes consecutive duplicates
 * i.e. 1123 -> 23 */
void remove_duplicates(std::vector<int> & arr)
{
	std::vector<int> newarr;
	for (unsigned int i = 0; i < arr.size(); ++i)
	{
		if (i == arr.size()-1 || arr[i] != arr[i+1])
			newarr.push_back(arr[i]);
		else
			++i;
	}
	arr = std::move(newarr);
}


/* Returns true if a part of the polygon is between the lines.*/
template <typename T, typename C1, typename C2, typename C3>
bool cutter(const polygon<T> & p, C1 & upres, C2 & downres, C3 & midres, T up, T down, T tolerance = 0.0)
{
	PositionState actstat, prevstat;
	polygon<T> newp;
	const point<T> * pprev_vertex = &(p.back());

	std::vector<int> up_intersections;
	std::vector<int> down_intersections;

	prevstat = ytoposstate(pprev_vertex->y, up, down, tolerance);

	/* Creates a new polygon newp. It has also intersections with up and down
	 * as vertices. The indexes of the intersections with up and down are stored
	 * in up_intersections and down_intersections */
	for (const auto & vertex : p)
	{
		const point<T> & prev_vertex = *pprev_vertex;


		actstat = ytoposstate(vertex.y, up, down, tolerance);

		if ((actstat == MID && prevstat == DOWN)       ||
			(actstat == DOWN && prevstat == MID)   ||
			(prevstat == DOWN && actstat == MIDUP) ||
			(prevstat == MIDUP && actstat == DOWN))
		{
			newp.push_back(intersection(prev_vertex, vertex, down));
			down_intersections.push_back(newp.size()-1);
		}
		else if ((actstat == MID && prevstat == UP)    ||
			(actstat == UP && prevstat == MID)     ||
			(prevstat == UP && actstat == MIDDOWN) ||
			(prevstat == MIDDOWN && actstat == UP))
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

		else if (prevstat == UP && actstat == MIDUP)
			up_intersections.push_back(newp.size());
		else if (prevstat == MIDUP && actstat == UP)
			up_intersections.push_back(newp.size()-1);

		else if (prevstat == DOWN && actstat == MIDDOWN)
			down_intersections.push_back(newp.size());
		else if (prevstat == MIDDOWN && actstat == DOWN)
			down_intersections.push_back(newp.size()-1);


		newp.push_back(vertex);

		prevstat = actstat;
		pprev_vertex = &vertex;
	}

	if (tolerance != 0.0)
	{
		/* If the refiner is enabled there might be a corner case:
		 * an intersection vector can be 1135. We have to remove consecutive
		 * duplicates(-> 35). In order to do so we must first transform the
		 * intersection indexes to be in the range [0, intersections.size()-1]
		 * to allow comparison */
		auto normalize_fn = [&](int & a)
		{
			a = newp.normalize_index(a);
		};
		std::for_each(up_intersections.begin(), up_intersections.end(), normalize_fn);
		std::for_each(down_intersections.begin(), down_intersections.end(), normalize_fn);

		remove_duplicates(up_intersections);
		remove_duplicates(down_intersections);
	}

	/* Sort intersections by the x coordinate */
	auto sortcriterion = [&](const int & a, const int & b)->bool
	{
		return newp[a].x < newp[b].x;
	};
	sort(up_intersections.begin(), up_intersections.end(), sortcriterion);
	sort(down_intersections.begin(), down_intersections.end(), sortcriterion);

	/* Split polygon by intersections */

	if (up_intersections.size() == 0 && down_intersections.size() == 0)
	{
		/* No intersections. The polygon must be entirely above, below,
		 * or in the middle of the lines */
		int polpos = UP | DOWN | MID;
		for (const auto & point:newp)
		{
			polpos &= ytoposstate(point.y, up, down, tolerance);
			if(polpos == UP || polpos == DOWN || polpos == MID)
				break;
		}
		if (polpos == MIDUP || polpos == MIDDOWN)
			polpos = MID;
		if (polpos == DOWN)
		{
			downres.push_back(newp);
			return false;
		}
		else if (polpos == UP)
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
	}
	else if (up_intersections.size() == 0 && down_intersections.size() == 4)
	{
		if (isvec4ordasc(down_intersections))
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
		if (isvec4ordasc(up_intersections))
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
		if (isvec4ordasc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[2], down_intersections[3], down_intersections[0]));

			if (isvec4ordasc(make_array(down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1])))
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
		if (isvec4ordasc(up_intersections))
		{
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
		}
		else
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			if (isvec4ordasc(make_array(up_intersections[1],down_intersections[1], down_intersections[0], up_intersections[0])))
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
		if (!isvec4ordasc(up_intersections) && !isvec4ordasc(down_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
			midres.push_back(myslice(newp, down_intersections[2], up_intersections[2], up_intersections[3], down_intersections[3]));
		}
		else if (isvec4ordasc(up_intersections) && !isvec4ordasc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[3], down_intersections[2], down_intersections[1]));
		}
		else if (isvec4ordasc(up_intersections) && isvec4ordasc(down_intersections))
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
