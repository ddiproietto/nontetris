#ifndef _CUTTER_H
#define _CUTTER_H

#include <vector>

#include "polygon.h"

template <typename T>
class Cutter
{
	enum PositionState {UP, MID, DOWN};
	T down, up;

	PositionState ytoposstate(T y);
public:
	std::vector<polygon<T>> upres;
	std::vector<polygon<T>> downres;
	std::vector<polygon<T>> midres;

	Cutter(T down, T up);
	bool cutbodyheight(const polygon<T> & p);
};

template <typename T>
Cutter<T>::Cutter(T down, T up):up(up),down(down)
{
}

template <typename T>
typename Cutter<T>::PositionState Cutter<T>::ytoposstate(T y)
{
	if (y < up)
		return UP;
	else if (y > down)
		return DOWN;
	else
		return MID;
}

template <typename T>
point<T> intersection(const point<T> & p1, const point<T> & p2, T y)
{
	point <T> ret;
	ret.y = y;
	ret.x = ((p2.x-p1.x)/(p2.y-p1.y))*(y-p1.y) + p1.x;
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

	for (int i = from; i < to; ++i)
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

template <typename T>
bool Cutter<T>::cutbodyheight(const polygon<T> & p)
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

		newp.push_back(vertex);

		actstat = ytoposstate(vertex.y);

		if(actstat == prevstat)
			continue;

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

	if (up_intersections.empty() && down_intersections.empty())
	{
		/* No intersections. The polygon must be entirely above, below,
		 * or in the middle of the lines */
		if (newp[0].y > down)
		{
			downres.push_back(newp);
		}
		else if (newp[0].y < up)
		{
			upres.push_back(newp);
		}
		else
		{
			midres.push_back(newp);
		}
		return false;
	}
	else if (up_intersections.empty() && down_intersections.size() == 2)
	{
		downres.push_back(myslice(newp, down_intersections[1], down_intersections[0]));
		midres.push_back(myslice(newp, down_intersections[0], down_intersections[1]));
	}
	
}

#endif //_CUTTER_H
