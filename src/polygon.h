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
#ifndef _POLYGON_H
#define _POLYGON_H

#include <vector>
#include <array>
#include <iterator>
#include <cmath>
#include <algorithm>

#include <limits>

template <class T>
struct point
{
	T x;
	T y;
	point()
	{
	}
	point(T _x, T _y):x(_x),y(_y)
	{
	}
	static T crossproduct(point<T> vec1, point<T> vec2)
	{
		return (vec1.x)*(vec2.y) - (vec1.y)*(vec2.x) ;
	}
	point<T> returnrotate(T angle) const
	{
		T sinrot = sin(angle);
		T cosrot = cos(angle);
		point <T> ret;

		ret.x = x*cosrot - y*sinrot;
		ret.y = x*sinrot + y*cosrot;
		return ret;
	}
	point<T> returntranslate(T t_x, T t_y) const
	{
		point <T> ret;

		ret.x = x + t_x;
		ret.y = y + t_y;
		return ret;
	}
	void rotate(T angle)
	{
		auto newp = returnrotate(angle);
		*this = newp;
	}
	void translate(T t_x, T t_y)
	{
		auto newp = returntranslate(t_x, t_y);
		*this = newp;
	}
	//dot product
	T operator*(const point<T> & p)
	{
		return p.x*x + p.y*y;
	}
	point<T> operator+(const point<T> & p)
	{
		point<T> ret = *this;
		ret.x += p.x;
		ret.y += p.y;
		return ret;
	}
	point<T> operator-(const point<T> & p)
	{
		point<T> ret = *this;
		ret.x -= p.x;
		ret.y -= p.y;
		return ret;
	}
	friend point<T> operator * (T scale, const point<T> & obj)
	{
		point<T> ret = obj;
		ret.x *= scale;
		ret.y *= scale;
		return ret;
	}
	#if !(defined(__CHEERP__)||defined(EMSCRIPTEN))
	friend std::ostream & operator<< (std::ostream & os, const point<T> & obj)
	{
		os<<'['<<obj.x<<", "<<obj.y<<']';
		return os;
	}
	#endif
};


//computes the minimum distance from the point p
//to the segment ab
template <class T>
T distseg(point<T> a, point<T> b, point<T> p)
{
	// coeff*ab = orthogonal projection on the ab line
	T coeff = ((p-b) * (a-b))/((a-b)*(a-b));

	if ( coeff > 1)
		return sqrt((p-a)*(p-a));
	else if (coeff < 0)
		return sqrt((p-b)*(p-b));
	else
	{
		point<T> p1 = coeff * ( a - b ) + b;
		point<T> dist = p - p1;
		return sqrt(dist*dist);
	}
}

template <class T = float>
class polygon
{
	std::vector<point<T>> vertices;
public:
	polygon(std::initializer_list<T> l) //UGLY CONSTRUCTOR
	{
		for(auto it = l.begin(); it != l.end();)
		{
			point<T> p;
			p.x = *it;
			it++;
			if(it != l.end())
			{
				p.y = *it;
				it++;
			}
			else
			{
				p.y = 0;
			}

			vertices.push_back(p);

		}
	}
	polygon(std::initializer_list<point<T>> l): vertices(l)
	{
	}
	polygon()
	{
		vertices.reserve(10);
	}
	size_t normalize_index(size_t n) const
	{
		size_t s = vertices.size();
		if (s==0)
			return n;
		return (n+s) % s;
	}
	point<T> & operator[](size_t n)
	{
		return vertices[normalize_index(n)];
	}
	const point<T> & operator[](size_t n) const
	{
		return vertices[normalize_index(n)];
	}

	size_t size() const
	{
		return vertices.size();
	}

	void add_vertex(point<T> p)
	{
		vertices.push_back(p);
	}

	void remove_vertex(size_t n)
	{
		vertices.erase(n);
	}

	auto begin() -> decltype(vertices.begin())
	{
		return vertices.begin();
	}
	auto end() -> decltype(vertices.end())
	{
		return vertices.end();
	}
	auto begin() const -> decltype(vertices.cbegin())
	{
		return vertices.cbegin();
	}
	auto end() const -> decltype(vertices.cend())
	{
		return vertices.cend();
	}

	bool isconvexcw()
	{
		size_t s = vertices.size();
		for(int i = 0; i < s; ++i)
		{
			T p = point<T>::crossproduct(vertices[i]-vertices[(i+1)%s], vertices[(i+2)%s]-vertices[(i+1)%s]);
			if (p > 0)
				return false;

		}
		return true;
	}

	void insert(size_t pos, const point<T> & obj)
	{
		vertices.insert(vertices.begin()+pos,obj);
	}

	void push_back(const point<T> & p)
	{
		vertices.push_back(p);
	}

	void push_back(point<T> && p)
	{
		vertices.push_back(p);
	}

	auto back() -> decltype(vertices.back())
	{
		return vertices.back();
	}

	auto back() const -> const point<T> &
	{
		const auto & v = vertices;
		return v.back();
	}

	void removealignedvertices()
	{
		/* Removes aligned vertices, i.e. vertices whose edges form an angle
		 * between 180+-7 degrees. Change costhetathreshold to change the angle
		 *
		 * Implementation:
		 * - find vertices to remove
		 * - sort them by angle (more aligned first)
		 * - erase them, but not all of them: we must guarantee that the polygon still has 3 vertices
		 */

		/*
		That's what I would write if I could use cos to init a constexpr
		constexpr T thetathreshold = 173.0*M_PI/180.0;
		constexpr T costhetathreshold = cos(thetathreshold);
		*/

		constexpr T costhetathreshold = -0.992546152;

		std::vector<std::pair<int, T>> markedvertices;

		size_t s = vertices.size();
		for (int i = 0; i < s; ++i)
		{
			point<T> segA = vertices[i] - vertices[(i+1)%s];
			point<T> segB = vertices[(i+2)%s] - vertices[(i+1)%s];

			T ps = segA*segB;

			T denom = fabs(sqrt(segA*segA)*sqrt(segB*segB));

			T costheta = ps/denom;

			if (costheta < costhetathreshold)
			{
				markedvertices.emplace_back(std::make_pair((i+1)%s, costheta));
			}
		}

		std::sort(markedvertices.begin(), markedvertices.end(), [](const std::pair<int,T> & v1, const std::pair<int,T> & v2)->bool{
			return std::get<1>(v1) > std::get<1>(v2);
		});

		if(s-markedvertices.size() < 3)
		{
			markedvertices.erase(markedvertices.begin()+(s-3), markedvertices.end());
		}

		// If only remove_if allowed me to use indexes instead of the element (rp)
		// I wouldn't have to do find_if and everything wouls have been better
		vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [&](const point<T> & rp)->bool{
			return find_if(markedvertices.begin(), markedvertices.end(), [&](const std::pair<int, T> & m)->bool{
				return vertices[std::get<0>(m)].x == rp.x && vertices[std::get<0>(m)].y == rp.y;
			}) != markedvertices.end();
		}), vertices.end());

	}

	void removetoonearvertices()
	{
		size_t s = vertices.size();
		for(int i = 0; i < s; ++i)
		{
			point<T> segment = vertices[i] - vertices[normalize_index(i-1)];
			T distsquared = segment*segment;

			if (distsquared < 1.19209289550781250000e-7F) // DISTANCE TOO SMALL (e.g. for box2d)
			{
				vertices.erase(vertices.begin()+normalize_index(i));
				i--;
				s--;
			}
		}
	}

	T area() const
	{
		T doublearea = 0.0;
		point<T> prev = vertices.back();
		for (auto p: vertices)
		{
			doublearea += prev.x*p.y-prev.y*p.x;

			prev = p;
		}
		return doublearea / 2;
	}

	void rotateangle(T angle)
	{
		for(auto & p: vertices)
		{
			p.rotate(angle);
		}
	}

	polygon<T> returnrotateangle(T angle)
	{
		polygon<T> ret = *this;
		ret.rotateangle(angle);
		return ret;
	}

	void translate(T x, T y)
	{
		for(auto & p: vertices)
		{
			p.translate(x, y);
		}
	}

	polygon<T> returntranslate(T x, T y)
	{
		polygon<T> ret = *this;
		ret.translate(x, y);
		return ret;
	}

	point<T> maxocccentroid() const
	{
		T xmax, xmin, ymax, ymin;
		xmin = ymin = std::numeric_limits<T>::infinity();
		xmax = ymax = - std::numeric_limits<T>::infinity();

		for (const auto & p: vertices)
		{
			xmin = std::min(p.x, xmin);
			ymin = std::min(p.y, ymin);
			xmax = std::max(p.x, xmax);
			ymax = std::max(p.y, ymax);
		}

		return point<T>(xmin+xmax/2, ymin+ymax/2);
	}
	// Returns the minimum distance from the point p to one of
	// the edges of the polygon
	T dist(point<T> p)
	{
		auto e1 = vertices.back();

		T mindist = std::numeric_limits<T>::infinity();

		for(auto & e2: vertices)
		{
			T tdist = distseg(e1, e2, p);
			mindist = std::min(mindist, tdist);
			e1 = e2;
		}
		return mindist;
	}

	//This is useful for testing purposes
	void arrayrotate(size_t pos)
	{
		rotate(vertices.begin(), vertices.begin()+normalize_index(pos), vertices.end());
	}

	#ifndef __CHEERP__
	friend std::ostream & operator<< (std::ostream & os, const polygon<T> & obj)
	{
		//Print everything but the last element
		std::copy(obj.begin(), obj.end()-1, std::ostream_iterator<point<T>>(os, ", "));
		//Print the last element
		os << obj.back();
		return os;
	}
	#endif /* __CHEERP__ */
};
#endif //_POLYGON_H
