#ifndef _CUTTER_H
#define _CUTTER_H

#include <vector>
#include <algorithm>

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

template <typename T, typename... Params>
polygon<T> myslice(const polygon<T> & orig, Params... params)
{
	polygon<T> ret;
	myslice_internal(orig, ret, params...);
	return ret;
}

static int signdiff(int a, int b)
{
	if (a > b)
		return 1;
	else
		return -1;
}

static int isvec4asc(const std::vector<int> & arr)
{
	int tmp = signdiff(arr[0],arr[1]) + signdiff(arr[1],arr[2]) + signdiff(arr[2],arr[3]);

	return (tmp < 0);
}

static int isvec4ordered(int arr0, int arr1, int arr2, int arr3)
{
	/* Due to a bug, in JS the whole thing returned always true, and always worked
	 * Right now the code is a mess. I have to investigate further */
	return true;

	int tmp = (arr0>arr1) + (arr1>arr2) + (arr2 > arr3) + (arr3 > arr0);

	if(tmp > 1)
		return false;
	return true;

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
		if(isvec4asc(down_intersections))
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
		if(isvec4asc(up_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, up_intersections[1], up_intersections[2], up_intersections[3], up_intersections[0]));
		}
		else
		{
			midres.push_back(myslice(newp, up_intersections[3], up_intersections[2]));
			midres.push_back(myslice(newp, up_intersections[2], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
		}
	}
	else if (up_intersections.size() == 2 && down_intersections.size() == 4)
	{
		upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
		if(isvec4asc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[2], down_intersections[3], down_intersections[0]));

			if(isvec4ordered(down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]))
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
		if(isvec4asc(up_intersections))
		{
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
		}
		else
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			if(isvec4ordered(up_intersections[1],down_intersections[1], down_intersections[0], up_intersections[0]))
			{
				midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
				midres.push_back(myslice(newp, up_intersections[3], up_intersections[2]));
			}
			else
			{
				midres.push_back(myslice(newp, down_intersections[2], up_intersections[0], up_intersections[1], down_intersections[3]));
				midres.push_back(myslice(newp, down_intersections[0], down_intersections[1]));
			}
		}
	}
	else if (up_intersections.size() == 4 && down_intersections.size() == 4)
	{
		if(!isvec4asc(up_intersections) && !isvec4asc(down_intersections))
		{
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[3], up_intersections[2], up_intersections[1]));
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], down_intersections[1]));
			midres.push_back(myslice(newp, down_intersections[2], up_intersections[2], up_intersections[3], down_intersections[3]));
		}
		else if(isvec4asc(up_intersections) && !isvec4asc(down_intersections))
		{
			downres.push_back(myslice(newp, down_intersections[3], down_intersections[2]));
			downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
			upres.push_back(myslice(newp, up_intersections[0], up_intersections[1]));
			upres.push_back(myslice(newp, up_intersections[2], up_intersections[3]));
			midres.push_back(myslice(newp, down_intersections[0], up_intersections[0], up_intersections[1], up_intersections[2], up_intersections[3], down_intersections[3], down_intersections[2], down_intersections[1]));
		}
		else if(isvec4asc(up_intersections) && isvec4asc(down_intersections))
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
