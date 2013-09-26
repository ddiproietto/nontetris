#ifndef _INPUT_HANDLER
#define _INPUT_HANDLER

#include <functional>


class InputHandler
{
	#ifdef __DUETTO__
	static bool k_down;
	static bool k_left;
	static bool k_right;
	static bool k_z;
	static bool k_x;
	#endif
public:
	InputHandler();
	void process_input(std::function<void()> exit, std::function<void()> left, std::function<void()> right, std::function<void()> down, std::function<void()> z, std::function<void()> x);
	#ifdef __DUETTO__
	static void keydown(client::KeyboardEvent * _e);
	static void keyup(client::KeyboardEvent * _e);
	#endif
};

#endif //_INPUT_HANDLER
