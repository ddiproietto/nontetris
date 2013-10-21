#ifndef _POLYGON_H
#define _POLYGON_H

#include <vector>
#include <array>
#include <iterator>
#include <cmath>

template <class T>
struct point
{
	T x;
	T y;
	static T crossproduct(point<T> p1, point<T> p2, point<T> p3)
	{
		return (p2.y-p1.y)*(p3.x-p2.x) -(p2.x-p1.x)*(p3.y-p2.y);
	}
	static T multmod(point<T> p1, point<T> p2, point<T> p3)
	{
		return sqrt(((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x))*((p3.y-p2.y)*(p3.y-p2.y)+(p3.x-p2.x)*(p3.x-p2.x)));
	}
	/*
	friend std::ostream & operator<< (std::ostream & os, point<T> obj)
	{
		os<<'('<<obj.x<<", "<<obj.y<<')';
		return os;
	}
	*/
};

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

	/*
	friend std::ostream & operator<< (std::ostream & os, const polygon<T> & obj)
	{
		std::copy(obj.vertices.begin(), obj.vertices.end(), std::ostream_iterator<point<T>>(os, " "));
		return os;
	}
	*/
};
#endif //_POLYGON_H
