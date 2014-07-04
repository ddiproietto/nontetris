#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#include <array>
#include <list>
#include "../src/myutil.h"
#include "../src/convexer.h"

//When std cout is supported hopefully we could merge the tests

auto tests = make_array(
		polygon <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }) //Z
		//polygon <float> ({{-1.42132, 0.7}, {-1.76777, 0.353553}, {-0.353553, -1.06066}, {0.353553, -0.353553}, {1.06066, -1.06066}, {1.76777, -0.353553}, {0.714213, 0.7}, {-0.00710678, 0.7}, {-0.353553, 0.353553}, {-0.7, 0.7}}) //Z CUTTED
		);

using namespace client;

template <typename T>
void printpoint(const point<T> & p)
{
	console.log("[", p.x, ", ", p.y, "]");
}

template <typename T>
void printpolygon(const polygon<T> & pol)
{
	for (auto it = pol.begin(); it != pol.end()-1; ++it)
	{
		printpoint(*it);
		console.log(",");
	}
	printpoint(pol.back());
}

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
			console.log(",");
		console.log("\t\t[");
		printpolygon (polygon);
		console.log("]");
		newarea += polygon.area();
	}
}


int webMain()
{
	console.log("[");

	/* We're in 2014 and the easiest way to avoid generating trailing commas is to use
	 * these variables. This fills my heart with furious rage */
	bool firsttest = true;
	int indextest = 0;

	for (auto & test: tests)
	{
		float oldarea, newarea = 0;

		std::vector<polygon<float>> result;

		//This rotates the polygon array, to test further the cutter
		//Change this number to perform additional testing
		test.arrayrotate(7);

		oldarea = test.area();

		convexer(test, result);

		if(firsttest)
			firsttest = false;
		else
			console.log(",");

		console.log("{");
		console.log("\t\"orig\": [");
		printpolygon(test);
		console.log("],");

		console.log("\t\"separateplots\": [");
		printeverypolygon_updatearea(result, newarea);
		console.log ("\t]");

		if (!floatequal(newarea, oldarea, 0.0001))
		{
			console.log(",\"newarea\":");
			console.log(newarea);
			console.log(",\"oldarea\":");
			console.log(oldarea);
		}
		console.log("}");
		++indextest;
	}
	console.log("]");
	return 0;
}
