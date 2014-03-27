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
#ifndef _PHYSIC_HANDLER_H
#define _PHYSIC_HANDLER_H

#include <functional>
#include "Box2D/Box2D.h"
#include "piece.h"


class PhysicHandler;

class PhysicPiece
{
	b2Body * ptr;
	enum PhysicPieceType {GROUND, LEFT, RIGHT, FALLING_PIECE, OLD_PIECE} type;
	void * otherdata;
public:
	PhysicPiece(b2Body * p = NULL):ptr(p)
	{}
	float getX()
	{
		b2Vec2 pos = ptr->GetPosition();
		return pos.x;
	}
	float getY()
	{
		b2Vec2 pos = ptr->GetPosition();
		return pos.y;
	}
	float getRot()
	{
		return ptr->GetAngle();
	}
	friend class PhysicHandler;
	
};
class PhysicHandler
{
	b2World world;
	float32 w_width;
	float32 w_height;
	b2Body * fallingpiece;
	const float stepInterval;
	bool accelerating;
	PhysicPiece rightwall, leftwall, groundwall;
	class PhysicHandlerContactListener : public b2ContactListener
	{
	public:
		bool callcollision;
		PhysicHandlerContactListener();
		void BeginContact(b2Contact* contact);
	} contactlistener;
public:
	PhysicHandler(float width, float height, double pstep = (1.0/60.0));
	PhysicPiece * createpiece(piece<float>, float, float, float, void *);
	void step(std::function<void(float x, float y)> cb);
	void debugprint();
	void drawbodies(std::function <void (float, float, float, void *)> draw);
	void piecerotate(float rot);
	void piecemove(float mov);
	void pieceaccelerate();
};
#endif //_PHYSIC_HANDLER_H
