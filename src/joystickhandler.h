#ifndef _JOYSTICK_HANDLER
#define _JOYSTICK_HANDLER
class JoystickHandler
{
public:
	struct JoystickValues
	{
		float axes[2];
		bool buttons[2];
	};
	static bool isJoystickPresent();
	static JoystickValues pollJoystick();
};
#endif //_JOYSTICK_HANDLER
