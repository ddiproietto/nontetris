#include <iostream>
#include <array>
#include <list>
#include "../src/myutil.h"
#include "../src/convexer.h"

auto tests = make_array(
		polygon<float>({{-2, -0.5}, {1, -0.5}, {-2, -0.224049}})
		);

using namespace std;

int main()
{
	cout << "[" << endl;

	/* We're in 2014 and the easiest way to avoid generating trailing commas is to use
	 * these variables. This fills my heart with furious rage */
	bool firsttest = true;
	int indextest = 0;

	for (auto & test: tests)
	{
		std::vector<polygon<float>> result;

		if(firsttest)
			firsttest = false;
		else
			cout << "," << endl;

		cout << "{" << endl;
		cout << "\t\"orig\": [";
		cout << test << "]," << endl;

		test.removealignedvertices();

		cout << "\t\"separateplots\": [" << endl; 
		cout << "\t\t[" << test << "]" << endl;
		cout << "\t]" << endl; 

		cout << "}" << endl;
		++indextest;
	}
	cout << "]" << endl;
}
