#include <iostream>
#include <array>
#include <tuple>
#include "../src/myutil.h"
#include "../src/polygon.h"

using namespace std;

auto tests = make_array(
		make_tuple(point<float>(1.0, 1.0), point<float>(2.0, 2.0), point<float>(1.0,2.0)),
		make_tuple(point<float>(1.0, 1.0), point<float>(2.0, 2.0), point<float>(5.0,6.0)),
		make_tuple(point<float>(1.0, 1.0), point<float>(2.0, 2.0), point<float>(-3.0,-2.0)),
		make_tuple(point<float>(1.0, 1.0), point<float>(2.0, 2.0), point<float>(-4.0,-2.0))
		);

int main()
{
	for (auto & test: tests)
	{
		auto dist = distseg(get<0>(test), get<1>(test), get<2>(test));
		cout << get<0>(test) << " " << get<1>(test) << " " << get<2>(test) << " " << dist << endl;
	}
}
