#include <iostream>
#include <array>
#include <list>
#include "../src/myutil.h"
#include "../src/convexer.h"

auto tests = make_array(
		polygon <float> ({ { -1.5,-1}, { 0.5,-1}, { 0.5,0}, { 1.5,0}, { 1.5,1}, { -0.5,1}, { -0.5,0}, { -1.5,0} }), //Z
		polygon <float> ({{-1.42132, 0.7}, {-1.76777, 0.353553}, {-0.353553, -1.06066}, {0.353553, -0.353553}, {1.06066, -1.06066}, {1.76777, -0.353553}, {0.714213, 0.7}, {-0.00710678, 0.7}, {-0.353553, 0.353553}, {-0.7, 0.7}}), //Z CUTTED
		polygon <float> ({{7.77741, 13}, {7.96568, 13.3525}, {7.08363, 13.8236}, {7.55479, 14.7057}, {6.67275, 15.1768}, {5.73043, 13.4127}, {6.61247, 12.9416}}), // Z CUTTED
		polygon <float>({{-0.500000119, 0.999998927}, {-0.499999911, 1.3038516e-07}, {-1.49999976, 3.91155481e-07}, {-1.49999988, -1.00000036}, {1.50000036, -1.00000012}, {1.49999976, -3.91155481e-07}, {0.499999911, -1.3038516e-07}, {0.500000179, 1.0000006}}),
		polygon <float>({ {1.50000036, -0.999999762}, {1.50000036, 4.47034836e-07}, {0.50000006, -4.76837158e-07}, {0.500000179, 1.00000072}, {-0.500000179, 0.999999702}, {-1.5, 1.00000072}, {-1.5, 4.61935997e-07}, {-0.50000006, 4.76837158e-07}, {-0.50000006, -0.732521474} }),
		polygon <float>({ {-2, -0.224048585}, {-2, -0.500000358}, {-0.99999994, -0.5}, {1, -0.500000238} }),
		polygon <float>({ {2.00000048, -0.292636335}, {-1.00000036, -0.499999881}, {-0.0717708543, -0.5} })
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

		if(firsttest)
			firsttest = false;
		else
			cout << "," << endl;

		cout << "{" << endl;
		cout << "\t\"orig\": [";
		cout << test << "]," << endl;

		result = convexer(test);

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
