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
#ifndef _CONVEXER_H
#define _CONVEXER_H

#include "polygon.h"

#include <limits>

template <class T = float>
//TODO:return rvalue reference?
std::vector<polygon<T> > convexer(polygon<T> p)
{
	/*
	std::cerr<<"OLDPOLYGON:";
	for(auto pt: p)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	*/
	size_t s = p.size();
	std::vector <size_t> concavity_points_indexes;
	std::vector <size_t> cutpoints_indexes;
	//Find concavity points
	for(int i = 0; i < s; ++i)
	{
		T pr = point<T>::crossproduct(p[i], p[i+1], p[i+2]);
		if (pr > 0)
		{
			concavity_points_indexes.push_back((i+1)%s);
		}

	}

	/*
	std::cerr<<"CONCAVITY POINTS:";
	for(auto pt: concavity_points_indexes)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	*/
	if(concavity_points_indexes.size() == 0)
		//There are no concavity points!
		//The polygon is convex
		return std::vector<polygon<T>>({p});
	if(concavity_points_indexes.size() == 2)
	{
		cutpoints_indexes = concavity_points_indexes;
	}
	else if (concavity_points_indexes.size() == 1)
	{
		cutpoints_indexes = concavity_points_indexes;
		int i = concavity_points_indexes[0];
		bool thisok1 = false, thisok2 = false, oldok1 = false, oldok2 = false;
		int num;

		for (num = 1; num < s; num++)
		{
			T np1 = point<T>::crossproduct(p[i-1], p[i], p[i+1+num]);
			T np2 = point<T>::crossproduct(p[i+1+num],p[i], p[i+1]);
			
			bool thisok1 = np1 < 0;
			bool thisok2 = np2 < 0;

			if((oldok1 && !thisok1) || (oldok2 && !thisok2))
			{
				int insert_before_index = p.normalize_index(i + 1 + num);
				int insert_after_index = p.normalize_index(insert_before_index - 1);
				double x0 = p[insert_before_index].x,
				       y0 = p[insert_before_index].y,
				       x1 = p[insert_after_index].x,
				       y1 = p[insert_after_index].y,
				       xp = p[i].x,
				       yp = p[i].y,
				       xa = p[i-1].x,
				       ya = p[i-1].y,
				       xb = p[i+1].x,
				       yb = p[i+1].y;
				
				double mu1 = ((y0-yb)*(xp-xb) - (x0-xb)*(yp-yb))/((x1-x0)*(yp-yb)-(y1-y0)*(xp-xb));

				float c1x = x0 + mu1*(x1-x0),
				      c1y = y0 + mu1*(y1-y0);

				point<T> newpoint({c1x,c1y});
				//std::cerr<<newpoint<<std::endl;
				
				p.insert(insert_before_index, point<T>({c1x,c1y}));
				cutpoints_indexes.push_back(insert_before_index);
				
				if(cutpoints_indexes[0] >= insert_before_index)
				{
					cutpoints_indexes[0]++;
				}
				break;
			}

			if (!oldok1 && thisok1)
				oldok1 = true;
			if (!oldok2 && thisok2)
				oldok2 = true;


		}
		s = p.size();
	}
	else
		;
		//TODO:signal error
		//cerr << "Convexer error: concavity_points > 2" <<std::endl;
	/*
	std::cerr<<"CUTPOINTS:";
	for(auto pt: cutpoints_indexes)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	*/
	
	polygon<T> polygonA, polygonB;
	bool doingA = true;
	for(int i = 0; i < s; i++)
	{
		if( i == cutpoints_indexes[0] || i == cutpoints_indexes[1])
		{
			doingA = !doingA;
			polygonA.add_vertex(p[i]);
			polygonB.add_vertex(p[i]);
		}
		else if(doingA)
			polygonA.add_vertex(p[i]);
		else if(!doingA)
			polygonB.add_vertex(p[i]);
	}
	polygonA.removealignedvertices();
	polygonB.removealignedvertices();

	// The convexer can create a point. But this point maybe too near to
	// another one. This should be avoided because it drives box2d crazy
	polygonA.removetoonearvertices();
	polygonB.removetoonearvertices();

	// Now, try to shift polygonB such that first element also belong to polygonA
	// Since that may not be possible, because of vertices removal,
	// let's put in the first place the polygonB vertex that has minimal distance
	// from polygonA
	// TODO: This is slow (A*B, number of the vertices)

	int mindistB_index;
	T mindist = std::numeric_limits<T>::infinity();
	for (int i = 0; i < polygonB.size(); ++i)
	{
		auto & pB = polygonB[i];
		T dist = polygonA.dist(pB);
		if (dist < mindist)
		{
			mindistB_index = i;
			mindist = dist;
		}
	}

	polygonB.arrayrotate(mindistB_index);

	std::vector<polygon<T>> ret;

	if (polygonA.size() >= 3)
		ret.push_back(polygonA);
	if (polygonB.size() >= 3)
		ret.push_back(polygonB);
	
	/*
	std::cerr<<"NEWPOLYGON:";
	for(auto pt: p)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	std::cerr<<"POLYGONA:";
	for(auto pt: polygonA)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	std::cerr<<"POLYGONB:";
	for(auto pt: polygonB)
	{
		std::cerr<<pt <<" ";
	}
	std::cerr<<std::endl;
	*/
	return ret;

}
#endif //_CONVEXER_H
