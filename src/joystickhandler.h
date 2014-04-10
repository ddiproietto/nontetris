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
#ifndef _JOYSTICK_HANDLER
#define _JOYSTICK_HANDLER
class JoystickHandler
{
public:
	struct JoystickValues
	{
		float axes[2];
		bool buttons[10];
	};
	static bool isJoystickPresent();
	static JoystickValues pollJoystick();
};
#endif //_JOYSTICK_HANDLER
