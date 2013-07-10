#ifndef _MYUTIL
#define _MYUTIL

#include <array>
#include <utility>

template<class T, class... Tail>
auto make_array(T head, Tail... tail) -> std::array<T, 1 + sizeof...(Tail)>
{
	std::array<T, 1 + sizeof...(Tail)> a = { head, tail ... };
	return a;
}

#endif //_MYUTIL
