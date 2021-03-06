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

#include "gameoptions.h"

class PhysicHandler;

struct PhysicPiece
{
	enum PhysicPieceType {GROUND, LEFT, RIGHT, FALLING_PIECE, OLD_PIECE, NEXT_PIECE};
private:
	PhysicPieceType type;
	b2Body * ptr;
	void * otherdata;
	bool iswall()
	{
		return type == GROUND || type == LEFT || type == RIGHT;
	}
	bool isfalling()
	{
		return type == FALLING_PIECE;
	}
	bool isnext()
	{
		return type == NEXT_PIECE;
	}
	bool isold()
	{
		return type == OLD_PIECE;
	}
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
	void * getUserData()
	{
		return otherdata;
	}
	void standstill()
	{
		ptr->SetLinearVelocity(b2Vec2(0, 0));
		ptr->SetAngularVelocity(0);
	}
	friend class PhysicHandler;
};

class PhysicHandler
{
	const GameOptions gameopt;
	b2World world;
	b2Body * fallingpiece;
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
	PhysicHandler(GameOptions _gameopt);
	PhysicPiece * createpiece(const piece<float> &, float, float, float, void *, PhysicPiece::PhysicPieceType, int, float angvel = 0.0f, float gravscale = 1.0F);
	void destroypiece(PhysicPiece * p);
	void step(int level, std::function<void(float x, float y)> cb);
	void debugprint();
	void iteratepieces(std::function <void (PhysicPiece *)> cb);
	void piecerotate(float rot);
	void piecemove(float mov);
	void pieceaccelerate();
	void getpieces_in_rect(float x0, float y0, float x1, float y1, std::function <void(PhysicPiece *)> cb);
	void gameover();
	void untagfallingpiece();
};
#endif //_PHYSIC_HANDLER_H
