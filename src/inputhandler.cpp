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
#include "NontetrisConfig.h"
#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
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

#include "inputhandler.h"

#include "joystickhandler.h"

#ifdef __DUETTO__
using namespace client;
#endif

InputHandler::KeyState InputHandler::act;
InputHandler::KeyState InputHandler::previntegrated;

InputHandler::InputHandler(GraphicToInput gti)
{
#ifdef __DUETTO__
	document.addEventListener("keydown", duetto::Callback(duetto_keydown));
	document.addEventListener("keyup", duetto::Callback(duetto_keyup));
#elif defined(EMSCRIPTEN)
	emscripten_set_keydown_callback(NULL, 0, 1, emscripten_keycallback);
	emscripten_set_keyup_callback(NULL, 0, 1, emscripten_keycallback);
#elif GLFW_VERSION_MAJOR == 3
	glfwwindow = gti.window;
	glfwSetKeyCallback(glfwwindow, glfw_keycallback);
#else
	glfwSetKeyCallback(glfw_keycallback);
#endif
	joystickpresent = jh.isJoystickPresent();
}

InputHandler::~InputHandler()
{
#ifdef __DUETTO__
	document.removeEventListener("keydown", duetto::Callback(duetto_keydown));
	document.removeEventListener("keyup", duetto::Callback(duetto_keyup));
#elif defined(EMSCRIPTEN)
	emscripten_set_keydown_callback(NULL, 0, 1, NULL);
	emscripten_set_keyup_callback(NULL, 0, 1, NULL);
#elif GLFW_VERSION_MAJOR == 3
	glfwSetKeyCallback(glfwwindow, NULL);
#else
	glfwSetKeyCallback(NULL);
#endif
}

void InputHandler::process_input(const std::function<void()> & exit, const std::function<void()> & left, const std::function<void()> & right, const std::function<void()> & down, const std::function<void()> & z, const std::function<void()> & x, const std::function<void()> & enter_press)
{
#ifndef __DUETTO__
	KeyState integrated = act;
#else
	KeyState integrated;
	integrated.k_esc = act.k_esc;
	integrated.k_down = act.k_down;
	integrated.k_left = act.k_left;
	integrated.k_right = act.k_right;
	integrated.k_z = act.k_z;
	integrated.k_x = act.k_x;
	integrated.k_enter = act.k_enter;
#endif

#if defined(__DUETTO__) || defined(EMSCRIPTEN)
#define WINDOW_OPENED true
#elif (GLFW_VERSION_MAJOR == 3)
#define WINDOW_OPENED !glfwWindowShouldClose(glfwwindow)
#else
#define WINDOW_OPENED glfwGetWindowParam( GLFW_OPENED )
#endif

	if(joystickpresent)
	{
		auto val = jh.pollJoystick();
		integrated.k_z |= val.buttons[0];
		integrated.k_x |= val.buttons[1];
		integrated.k_enter |= val.buttons[9];
		integrated.k_left |= val.axes[0] < -0.25;
		integrated.k_right |= val.axes[0] > 0.25;
		integrated.k_down |= val.axes[1] < -0.25;
	}

	if(integrated.k_esc || ! WINDOW_OPENED)
		exit();
	if(integrated.k_down)
	{
		down();
	}
	if(integrated.k_left)
	{
		left();
	}
	if(integrated.k_right)
	{
		right();
	}
	if(integrated.k_z)
	{
		z();
	}
	if(integrated.k_x)
	{
		x();
	}
	if(integrated.k_enter && !previntegrated.k_enter)
	{
		//We are only interested in events
		enter_press();
	}
	previntegrated = integrated;
}

void InputHandler::keyset(int key, bool setto)
{
#if defined(__DUETTO__) || defined(EMSCRIPTEN)
#define NONTETRIS_ESC 27
#define NONTETRIS_LEFT 37
#define NONTETRIS_RIGHT 39
#define NONTETRIS_DOWN 40
#define NONTETRIS_ENTER 13
#else
#if (GLFW_VERSION_MAJOR == 3)
#define NONTETRIS_ESC GLFW_KEY_ESCAPE
#else
#define NONTETRIS_ESC GLFW_KEY_ESC
#endif
#define NONTETRIS_LEFT GLFW_KEY_LEFT
#define NONTETRIS_RIGHT GLFW_KEY_RIGHT
#define NONTETRIS_DOWN GLFW_KEY_DOWN
#define NONTETRIS_ENTER GLFW_KEY_ENTER
#define NONTETRIS_KP_ENTER GLFW_KEY_KP_ENTER
#endif
	switch (key)
	{
	case NONTETRIS_ESC:
		act.k_esc = setto;
		break;
	case NONTETRIS_LEFT:
		act.k_left = setto;
		break;
	case NONTETRIS_RIGHT:
		act.k_right = setto;
		break;
	case NONTETRIS_DOWN:
		act.k_down = setto;
		break;
	case 'Z':
	case 'W':
	case 'Y':
		act.k_z = setto;
		break;
	case 'X':
		act.k_x = setto;
		break;
#ifdef NONTETRIS_KP_ENTER
	case NONTETRIS_KP_ENTER:
#endif
	case NONTETRIS_ENTER:
		act.k_enter = setto;
		break;
	}
}

#ifdef __DUETTO__
void InputHandler::duetto_keyup(KeyboardEvent * e)
{
	keyset(e->get_keyCode(), false);
}

void InputHandler::duetto_keydown(KeyboardEvent * e)
{
	int key = e->get_keyCode();
	keyset(e->get_keyCode(), true);
	if (37 <= key && key <= 40) //ARROWS
	{
		//AVOID SCROLLING
		e->preventDefault();
	}
}

#elif defined(EMSCRIPTEN)
int InputHandler::emscripten_keycallback(int action, const EmscriptenKeyboardEvent *e, void *userData)
{
	bool setto;
	if (action == EMSCRIPTEN_EVENT_KEYDOWN)
		setto = true;
	else if (action == EMSCRIPTEN_EVENT_KEYUP)
		setto = false;
	else
		//anything else must be ignored
		return 0;
	keyset((int)e->keyCode, setto);
	if(37 <= e->keyCode && e->keyCode <= 40) //ARROWS
	{
		//AVOID SCROLLING
		return 1;
	}
	return 0;
}

#else
#if GLFW_VERSION_MAJOR == 3
void InputHandler::glfw_keycallback(GLFWwindow * window, int key, int scancode, int action, int mods)
#else
void InputHandler::glfw_keycallback(int key, int action)
#endif
{
	bool setto;
	if (action == GLFW_PRESS)
		setto = true;
	else if (action == GLFW_RELEASE)
		setto = false;
	else
		//GLFW_REPEAT must be ignored
		return;

	keyset(key, setto);
}
#endif
