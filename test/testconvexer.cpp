#include <iostream>
#include <array>
#include <list>
#include "../src/myutil.h"
#include "../src/convexer.h"

auto tests = make_array(
		polygon <float> ({{-1.42132, 0.7}, {-1.76777, 0.353553}, {-0.353553, -1.06066}, {0.353553, -0.353553}, {1.06066, -1.06066}, {1.76777, -0.353553}, {0.714213, 0.7}, {-0.00710678, 0.7}, {-0.353553, 0.353553}, {-0.7, 0.7}}) //Z CUTTED
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

		std::vector<polygon<float>> result;

		//This rotates the polygon array, to test further the cutter
		//Change this number to perform additional testing
		test.arrayrotate(7);

		oldarea = test.area();

		result = convexer(test);

		if(firsttest)
			firsttest = false;
		else
			cout << "," << endl;

		cout << "{" << endl;
		cout << "\t\"orig\": [";
		cout << test << "]," << endl;

		cout << "\t\"separateplots\": [" << endl; 
		printeverypolygon_updatearea(result, newarea);
		cout << "\t]" << endl; 

		if (!floatequal(newarea, oldarea, 0.0001))
			cerr << newarea << "!=" << oldarea << " AREA MISMATCH FOR TEST "<< indextest << endl;
		cout << "}" << endl;
		++indextest;
	}
	cout << "]" << endl;
}
