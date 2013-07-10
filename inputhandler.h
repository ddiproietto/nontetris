#ifndef _INPUT_HANDLER
#define _INPUT_HANDLER

#include <functional>

#include <GL/glfw.h>

void process_input(std::function<void()> exit, std::function<void()> left, std::function<void()> right, std::function<void()> down, std::function<void()> z, std::function<void()> x)
{
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
}
#endif //_INPUT_HANDLER
