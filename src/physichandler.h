#ifndef _PHYSIC_HANDLER_H
#define _PHYSIC_HANDLER_H

#include <functional>
#include "Box2D/Box2D.h"
#include "piece.h"

struct userData
{
	enum {GROUND, LEFT, RIGHT, FALLING_PIECE, OLD_PIECE} type;
	void * otherdata;
	userData():otherdata(NULL)
	{
	}
};

class PhysicHandler;

class PhysicPiece
{
	b2Body * ptr;
	userData ud;
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
	class MyContactListener : public b2ContactListener
	{
	public:
		MyContactListener();
		void BeginContact(b2Contact* contact);
		bool callcollision;
	}contactlistener;
	userData udright, udleft, udground;
public:
	PhysicHandler(float width, float height);
	PhysicPiece * createpiece(piece<float>, float, float, float, void *);
	void step(std::function<void(float x, float y)> cb);
	void debugprint();
	void drawbodies(std::function <void (float, float, float, void *)> draw);
	void piecerotate(float rot);
	void piecemove(float mov);
	void pieceaccelerate();
};
#endif //_PHYSIC_HANDLER_H
