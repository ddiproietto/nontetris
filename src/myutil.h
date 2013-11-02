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
