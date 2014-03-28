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
	static T crossproduct(point<T> p1, point<T> p2, point<T> p3)
	{
		return (p2.y-p1.y)*(p3.x-p2.x) -(p2.x-p1.x)*(p3.y-p2.y);
	}
	static T multmod(point<T> p1, point<T> p2, point<T> p3)
	{
		return sqrt(((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x))*((p3.y-p2.y)*(p3.y-p2.y)+(p3.x-p2.x)*(p3.x-p2.x)));
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
	#if !(defined(__DUETTO__)||defined(EMSCRIPTEN))
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
			T p = point<T>::crossproduct(vertices[i], vertices[(i+1)%s], vertices[(i+2)%s]);
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
		size_t s = vertices.size();
		for(int i = 0; i < s; ++i)
		{
			T pv = point<T>::crossproduct(vertices[i], vertices[(i+1)%s], vertices[(i+2)%s]);
			T abyb = point<T>::multmod(vertices[i], vertices[(i+1)%s], vertices[(i+2)%s]);
			if(!(pv/abyb > 0.1 || pv/abyb <-0.1)) //DISCARD POINT IF >173 DEGREES
			{
				vertices.erase(vertices.begin()+normalize_index(i+1));
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

	#ifndef __DUETTO__
	friend std::ostream & operator<< (std::ostream & os, const polygon<T> & obj)
	{
		//Print everything but the last element
		std::copy(obj.begin(), obj.end()-1, std::ostream_iterator<point<T>>(os, ", "));
		//Print the last element
		os << obj.back();
		return os;
	}
	#endif /* __DUETTO__ */
};
#endif //_POLYGON_H
