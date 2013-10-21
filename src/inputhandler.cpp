#include "NontetrisConfig.h"
#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#else
#define GLEW_NO_GLU
#include <GL/glew.h>
#if (USE_GLFW_VERSION==3)
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif
#endif
#include "inputhandler.h"


#ifdef __DUETTO__
using namespace client;

bool InputHandler::k_down = false;
bool InputHandler::k_left = false;
bool InputHandler::k_right = false;
bool InputHandler::k_z = false;
bool InputHandler::k_x = false;

InputHandler::InputHandler()
{
	/*
	auto canvas = static_cast<client::HTMLCanvasElement*>(client::document.getElementById("glcanvas"));
	canvas->addEventListener("keydown",Callback(keydown));
	canvas->addEventListener("keyup",Callback(keyup));
	*/
	document.addEventListener("keydown",Callback(keydown));
	document.addEventListener("keyup",Callback(keyup));

}
#elif GLFW_VERSION_MAJOR == 3
InputHandler::InputHandler(GLFWwindow * w):glfwwindow(w)
{
}
#else
InputHandler::InputHandler()
{
}
#endif

void InputHandler::process_input(const std::function<void()> & exit, const std::function<void()> & left, const std::function<void()> & right, const std::function<void()> & down, const std::function<void()> & z, const std::function<void()> & x)
{
#ifdef __DUETTO__
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

#else
	#if (GLFW_VERSION_MAJOR == 3)
	#define WINDOW glfwwindow,
	#define WINDOW_OPENED !glfwWindowShouldClose(glfwwindow)
	#define GLFW_KEY_ESC GLFW_KEY_ESCAPE
	#else
	#define WINDOW
	#define WINDOW_OPENED glfwGetWindowParam( GLFW_OPENED )
	#endif
	if(glfwGetKey(WINDOW GLFW_KEY_ESC ) || ! WINDOW_OPENED)
		exit();
	if(glfwGetKey(WINDOW GLFW_KEY_LEFT))
	{
		left();
	}
	if(glfwGetKey(WINDOW GLFW_KEY_RIGHT))
	{
		right();
	}
	if(glfwGetKey(WINDOW GLFW_KEY_DOWN))
	{
		down();
	}
	if(glfwGetKey(WINDOW 'Z') || glfwGetKey(WINDOW 'W') || glfwGetKey(WINDOW 'Y'))
	{
		z();
	}
	if(glfwGetKey(WINDOW 'X'))
	{
		x();
	}
#endif
}

#ifdef __DUETTO__
void InputHandler::keyup(KeyboardEvent * e)
{
	if(e->get_keyCode() == 90)//Z
		k_z = false;
	if(e->get_keyCode() == 88)//X
		k_x = false;
	if(e->get_keyCode() == 40)//DOWN
		k_down = false;
	if(e->get_keyCode() == 37)//LEFT
		k_left = false;
	if(e->get_keyCode() == 39)//RIGHT
		k_right = false;

}

void InputHandler::keydown(KeyboardEvent * e)
{
	if(e->get_keyCode() == 90)//Z
		k_z = true;
	if(e->get_keyCode() == 88)//X
		k_x = true;
	if(e->get_keyCode() == 40)//DOWN
		k_down = true;
	if(e->get_keyCode() == 37)//LEFT
		k_left = true;
	if(e->get_keyCode() == 39)//RIGHT
		k_right = true;
}
#endif
