#ifndef _GAMEOPTIONS_H
#define _GAMEOPTIONS_H

struct GameOptions
{
	// Controls which type of game:
	// -cutting: try to remove as many lines as possible
	// -stack:   try to put as many pieces as possible in the screen
	enum GameType {CUTTING, STACK} gametype;

	float rows;
	float columns;
	float rowwidth;

	// The area of a row above which a cut happens
	double cuttingrowarea;

	// The frequency at which the completion bars are updated
	// 1.0 = once per frame
	// 0.2 = once every 5 frames (good choice)
	// 0.1 = once every 10 frames
	float updatebarsfreq;

	// The duration of a physic step in seconds
	// i.e. 1.0/60.0
	double physicstep;

	//Graphic options
	int width;
	int height;
	bool fullscreen;
	int piecesAA;
};

#endif // _GAMEOPTIONS_H
