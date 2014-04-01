#include "joystickhandler.h"

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

bool JoystickHandler::isJoystickPresent()
{
	//TODO: should return true only if it has two axes and two buttons
#ifdef __DUETTO__
	return false;
#elif defined(EMSCRIPTEN)
	return false;
#elif GLFW_VERSION_MAJOR == 3
	return glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE;
#else
	return glfwGetJoystickParam(GLFW_JOYSTICK_1, GLFW_PRESENT) == GL_TRUE;
#endif
}

JoystickHandler::JoystickValues JoystickHandler::pollJoystick()
{
	JoystickValues ret = {0.0, 0.0, false, false, false, false, false, false, false, false, false, false};
#ifdef __DUETTO__
#elif defined(EMSCRIPTEN)
#elif GLFW_VERSION_MAJOR == 3
	int numbuttons;
	int numaxes;
	const unsigned char * pbuttons;
	const float * paxes;
	pbuttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numbuttons);
	paxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &numaxes);
	if(numbuttons >= 1)
		ret.buttons[0] = pbuttons[0] == GLFW_PRESS;
	if(numbuttons >= 2)
		ret.buttons[1] = pbuttons[1] == GLFW_PRESS;
	if(numbuttons >= 10)
		ret.buttons[9] = pbuttons[9] == GLFW_PRESS;
	if(numaxes >= 1)
		ret.axes[0] = paxes[0];
	if(numaxes >= 2)
		ret.axes[1] = paxes[1];
#else
	unsigned char buttons[10];
	glfwGetJoystickButtons(GLFW_JOYSTICK_1, buttons,
			sizeof(buttons)/sizeof(buttons[0]));
	glfwGetJoystickPos(GLFW_JOYSTICK_1, ret.axes,
			sizeof(ret.axes)/sizeof(ret.axes[0]));

	ret.buttons[0] = buttons[0] == GLFW_PRESS;
	ret.buttons[1] = buttons[1] == GLFW_PRESS;
	ret.buttons[9] = buttons[9] == GLFW_PRESS;
#endif
	return ret;
}
