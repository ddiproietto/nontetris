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
#else
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif
#endif
#include "inputhandler.h"

#ifdef __DUETTO__
using namespace client;
#endif

bool InputHandler::k_esc = false;
bool InputHandler::k_down = false;
bool InputHandler::k_left = false;
bool InputHandler::k_right = false;
bool InputHandler::k_z = false;
bool InputHandler::k_x = false;

InputHandler::InputHandler(GraphicToInput gti)
{
#ifdef __DUETTO__
	document.addEventListener("keydown", Callback(keydown));
	document.addEventListener("keyup", Callback(keyup));
#elif GLFW_VERSION_MAJOR == 3
	glfwwindow = gti.window;
	glfwSetKeyCallback(glfwwindow, keycallback);
#else
	glfwSetKeyCallback(keycallback);
#endif
}

InputHandler::~InputHandler()
{
#ifdef __DUETTO__
	document.removeEventListener("keydown", &Callback(keydown));
	document.removeEventListener("keyup", &Callback(keyup));
#elif GLFW_VERSION_MAJOR == 3
	glfwSetKeyCallback(glfwwindow, NULL);
#else
	glfwSetKeyCallback(NULL);
#endif
}

void InputHandler::process_input(const std::function<void()> & exit, const std::function<void()> & left, const std::function<void()> & right, const std::function<void()> & down, const std::function<void()> & z, const std::function<void()> & x)
{
#ifdef __DUETTO__
#define WINDOW_OPENED true
#elif (GLFW_VERSION_MAJOR == 3)
#define WINDOW_OPENED !glfwWindowShouldClose(glfwwindow)
#else
#define WINDOW_OPENED glfwGetWindowParam( GLFW_OPENED )
#endif
	if(k_esc || ! WINDOW_OPENED)
		exit();
	if(k_down)
	{
		down();
	}
	if(k_left)
	{
		left();
	}
	if(k_right)
	{
		right();
	}
	if(k_z)
	{
		z();
	}
	if(k_x)
	{
		x();
	}
}

void InputHandler::keyset(int key, bool setto)
{
#ifdef __DUETTO__
#define NONTETRIS_ESC 27
#define NONTETRIS_LEFT 37
#define NONTETRIS_RIGHT 39
#define NONTETRIS_DOWN 40
#else
#if (GLFW_VERSION_MAJOR == 3)
#define NONTETRIS_ESC GLFW_KEY_ESCAPE
#else
#define NONTETRIS_ESC GLFW_KEY_ESC
#endif
#define NONTETRIS_LEFT GLFW_KEY_LEFT
#define NONTETRIS_RIGHT GLFW_KEY_RIGHT
#define NONTETRIS_DOWN GLFW_KEY_DOWN
#endif
	switch (key)
	{
	case NONTETRIS_ESC:
		k_esc = setto;
		break;
	case NONTETRIS_LEFT:
		k_left = setto;
		break;
	case NONTETRIS_RIGHT:
		k_right = setto;
		break;
	case NONTETRIS_DOWN:
		k_down = setto;
		break;
	case 'Z':
	case 'W':
	case 'Y':
		k_z = setto;
		break;
	case 'X':
		k_x = setto;
		break;
	}
}

#ifdef __DUETTO__
void InputHandler::keyup(KeyboardEvent * e)
{
	keyset(e->get_keyCode(), false);
}

void InputHandler::keydown(KeyboardEvent * e)
{
	int key = e->get_keyCode();
	keyset(e->get_keyCode(), true);
	if (37 <= key && key <= 40) //ARROWS
	{
		//AVOID SCROLLING
		e->preventDefault();
	}
}

#else
#if GLFW_VERSION_MAJOR == 3
void InputHandler::keycallback(GLFWwindow * window, int key, int scancode, int action, int mods)
#else
void InputHandler::keycallback(int key, int action)
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
