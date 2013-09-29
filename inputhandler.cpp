#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#else
#include <GL/glfw.h>
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
#else
InputHandler::InputHandler()
{
}
#endif

void InputHandler::process_input(std::function<void()> exit, std::function<void()> left, std::function<void()> right, std::function<void()> down, std::function<void()> z, std::function<void()> x)
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
	if(glfwGetKey( GLFW_KEY_ESC ) || !glfwGetWindowParam( GLFW_OPENED ))
		exit();
	if(glfwGetKey( GLFW_KEY_LEFT))
	{
		left();
	}
	if(glfwGetKey( GLFW_KEY_RIGHT))
	{
		right();
	}
	if(glfwGetKey( GLFW_KEY_DOWN))
	{
		down();
	}
	if(glfwGetKey('Z') || glfwGetKey('W') || glfwGetKey('Y'))
	{
		z();
	}
	if(glfwGetKey('X'))
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
