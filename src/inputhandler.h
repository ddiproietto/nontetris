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
#ifndef _INPUT_HANDLER
#define _INPUT_HANDLER

#include <functional>

#ifndef __DUETTO__
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#define GLFW_NO_GLU
#include <GL/glfw.h>
#endif
#endif

#include "graphictoinput.h"

class InputHandler
{
	#ifdef __DUETTO__
	static bool k_down;
	static bool k_left;
	static bool k_right;
	static bool k_z;
	static bool k_x;
	#elif GLFW_VERSION_MAJOR == 3
	GLFWwindow * glfwwindow;
	#endif

	#ifdef __DUETTO__
	static void keydown(client::KeyboardEvent * _e);
	static void keyup(client::KeyboardEvent * _e);
	#endif
public:
	InputHandler(GraphicToInput);
	void process_input(const std::function<void()> & exit, const std::function<void()> & left, const std::function<void()> & right, const std::function<void()> & down, const std::function<void()> & z, const std::function<void()> & x);
};

#endif //_INPUT_HANDLER
