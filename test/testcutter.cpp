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
#include <iostream>
#include <array>
#include <list>
#include "../src/myutil.h"
#include "../src/cutter.h"

struct testcase
{
	polygon<float> p;
	float up, down;
	testcase(const polygon<float> & _p, float _up, float _down):p(_p), up(_up), down(_down)
	{
	}
};

auto tests = make_array(
		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, 1, 2), // NO INTERSECTION UP
		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, -2, -1), // NO INTERSECTION DOWN
		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, -1, 1), // NO INTERSECTION MID

		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, -1, 0), // UP 0 DOWN 2
		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, 0, 1), // UP 2 DOWN 0

		testcase({{ -2,-0.5}, { 2,-0.5}, { 2,0.5}, { -2,0.5}}, -0.25, 0.25), // UP 2 DOWN 2

		testcase({ {-3, 0}, {-2, -1}, {0, 1}, {2, -1}, {3, 0}, {0, 2} }, -2, 0.1), // UP 0 DOWN 4 ASC
		testcase({ {0, -2}, {3, 0}, {2, 1}, {0, -1}, {-2, 1}, {-3, 0} }, -2, 0.1), // UP 0 DOWN 4 DESC

		testcase({ {-3, 0}, {-2, -1}, {0, 1}, {2, -1}, {3, 0}, {0, 2} }, -0.1, 2), // UP 4 DOWN 0 ASC
		testcase({ {0, -2}, {3, 0}, {2, 1}, {0, -1}, {-2, 1}, {-3, 0} }, -0.1, 2), // UP 4 DOWN 0 DESC

		testcase({ {0, -2}, {3, 0}, {2, 1}, {0, -1}, {-2, 1}, {-3, 0} }, -1.1, 0.1), // UP 2 DOWN 4 DESC
		testcase({ {-6, -3}, {-5, -4}, {0, 1}, {2, -1}, {3, 0}, {0, 2} }, -2, 0.1), // UP 2 DOWN 4 ASC
		testcase({ {-3, 0}, {-2, -1}, {0, 1}, {5, -4}, {6, -3}, {0, 2} }, -2, 0.1), // UP 2 DOWN 4 ASC

		testcase({ {-3, 0}, {-2, -1}, {0, 1}, {2, -1}, {3, 0}, {0, 2} }, -0.1, 1.1), // UP 4 DOWN 2 ASC
		testcase({ {0, -2}, {3, 0}, {2, 1}, {0, -1}, {-5, 4}, {-6, 3} }, 0.1, 2), // UP 4 DOWN 2 DESC
		testcase({ {0, -2}, {6, 3}, {5, 4}, {0, -1}, {-2, 1}, {-3, 0} }, 0.1, 2), // UP 4 DOWN 2 DESC

		testcase({ {-3, 0}, {-2, -1}, {0, 1}, {2, -1}, {3, 0}, {0, 2} }, -0.9, 0.1), // UP 4 ASC DOWN 4 ASC
		testcase({ {0, -2}, {3, 0}, {2, 1}, {0, -1}, {-2, 1}, {-3, 0} }, -0.9, 0.1), // UP 4 DESC DOWN 4 DESC

		testcase(polygon <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }).returnrotateangle(-M_PI/4), -0.7, 0.7), // UP 4 ASC DOWN 4 DESC

//TEST FOR REFINING
		testcase({ { -1.5,-1}, { 1.5,-1}, { 1.5,1}, { 0.5,1}, { 0.5,0}, { -1.5,0} }, 0, 1),
		testcase({ {0, -2}, {1, -1}, {0, 0}, {-1, -1} }, 0, 1),
		testcase(polygon <float> ({ { -0.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -1.5,1}, { -1.5,0}, { -0.5,0} }).returnrotateangle(-M_PI/2 -M_PI/16 -M_PI/32), -0.7, 0.7),
		testcase(polygon <float> ({ { -0.5,-1}, { 1.5,-1}, { 1.5,0}, { 0.5,0}, { 0.5,1}, { -1.5,1}, { -1.5,0}, { -0.5,0} }).returnrotateangle(-M_PI/2 + 66*M_PI/128).returntranslate(0, 0.5).returnrotateangle(26*M_PI/128), 0, 1),
		testcase(polygon <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }).returnrotateangle(-2.06140161).returntranslate(6.84805489, 13.3825932), 12, 13),
		testcase(polygon <float> ({ {-1.00000012, -0.50000006}, {1.00000012, -0.50000006}, {2.00000024, -0.50000006}, {2.00000072, -0.292636693} }).returnrotateangle(-0.140856937).returntranslate(6.15480137, 12.484972), 11, 12),
		testcase(polygon <float> ({ {-0.113753438, -0.99999994}, {0.499999821, -0.999999881}, {1.49999952, -0.999999821}, {1.49999952, 5.96046448e-08}, {1.4999994, 0.999999642}, {0.499999791, 0.999999642}, {0.499999851, 2.23517418e-08}, {-0.00484643038, 1.87836122e-07} }).returnrotateangle(-1.08976233).returntranslate(4.63231564, 16.9756145), 17, 18)

		);

using namespace std;

template <typename T, typename T2>
bool floatequal(T a, T b, T2 toll)
{
	if (a == 0)
		return a == b;
	return fabs((a - b)/a) < toll;
}

template <class ListOfPolygons, typename AreaType>
void printeverypolygon_updatearea(ListOfPolygons res, AreaType & newarea)
{
	bool firstpol = true;
	for (const auto & polygon: res)
	{
		if(firstpol)
			firstpol = false;
		else
			cout << ",";
		cout << "\t\t[" << polygon << "]" <<endl;
		newarea += polygon.area();
	}
}

int main()
{
	cout << "[" << endl;

	/* We're in 2014 and the easiest way to avoid generating trailing commas is to use
	 * these variables. This fills my heart with furious rage */
	bool firsttest = true;
	int indextest = 0;

	for (auto & test: tests)
	{
		float oldarea, newarea = 0;

		list<polygon<float>> upres, midres, downres;

		//This rotates the polygon array, to test further the cutter
		//Change this number to perform additional testing
		test.p.arrayrotate(0);

		oldarea = test.p.area();

		cutter(test.p, upres, downres, midres, test.up, test.down, 0.1F);

		if(firsttest)
			firsttest = false;
		else
			cout << "," << endl;

		cout << "{" << endl;
		cout << "\t\"orig\": [";
		cout << test.p << "]," << endl;
		cout << "\t\"lineup\": " << test.up <<"," << endl; 
		cout << "\t\"linedown\": " << test.down <<"," << endl; 

		cout << "\t\"up\": [" << endl; 
		printeverypolygon_updatearea(upres, newarea);
		cout << "\t]," << endl; 

		cout << "\t\"mid\": [" << endl; 
		printeverypolygon_updatearea(midres, newarea);
		cout << "\t]," << endl; 

		cout << "\t\"down\": [" << endl; 
		printeverypolygon_updatearea(downres, newarea);
		cout << "\t]" << endl; 

		if (!floatequal(newarea, oldarea, 0.0001))
			cerr << newarea << "!=" << oldarea << " AREA MISMATCH FOR TEST "<< indextest << endl;
		cout << "}" << endl;
		++indextest;
	}
	cout << "]" << endl;
}
