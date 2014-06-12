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

#ifdef __CHEERP__
#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#elif defined(EMSCRIPTEN)
#include "emscripten/html5.h"
#else
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#define GLFW_NO_GLU
#include <GL/glfw.h>
#endif
#endif

#include "graphictoinput.h"

#include "joystickhandler.h"

class InputHandler
{
	bool joystickpresent;
	JoystickHandler jh;

	struct KeyState
	{
		bool k_esc;
		bool k_down;
		bool k_left;
		bool k_right;
		bool k_z;
		bool k_x;
		bool k_enter;
	};

	static KeyState act, previntegrated;
	#if GLFW_VERSION_MAJOR == 3
	GLFWwindow * glfwwindow;
	#endif

	#ifdef __CHEERP__
	static void cheerp_keydown(client::KeyboardEvent * _e);
	static void cheerp_keyup(client::KeyboardEvent * _e);
	#elif defined(EMSCRIPTEN)
	static int emscripten_keycallback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData);
	#elif GLFW_VERSION_MAJOR == 3
	static void glfw_keycallback(GLFWwindow * window, int key, int scancode, int action, int mods);
	#else
	static void glfw_keycallback(int key, int action);
	#endif
	static void keyset(int key, bool setto);
public:
	InputHandler(GraphicToInput);
	~InputHandler();
	void process_input(const std::function<void()> & exit, const std::function<void()> & left, const std::function<void()> & right, const std::function<void()> & down, const std::function<void()> & z, const std::function<void()> & x, const std::function<void()> & enter_press);
};

#endif //_INPUT_HANDLER
