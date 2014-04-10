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
