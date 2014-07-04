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
#ifdef __CHEERP__
#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#include <stdio.h>
#endif
#include "physichandler.h"
#include "NontetrisConfig.h"

#ifndef __CHEERP__
#include <iostream>
#endif
#include <algorithm>
#include <set>
#include <list>

#include "polygon.h"

PhysicHandler::PhysicHandlerContactListener::PhysicHandlerContactListener():callcollision(false)
{
}
void PhysicHandler::PhysicHandlerContactListener::BeginContact(b2Contact* contact)
{
	PhysicPiece * bodyUserDataA = (PhysicPiece *) contact->GetFixtureA()->GetBody()->GetUserData();
	PhysicPiece * bodyUserDataB = (PhysicPiece *) contact->GetFixtureB()->GetBody()->GetUserData();
	bool isnA;

	if((isnA=(bodyUserDataA->type != PhysicPiece::FALLING_PIECE)) && bodyUserDataB->type != PhysicPiece::FALLING_PIECE)
		return;

	if(isnA)
		std::swap(bodyUserDataA,bodyUserDataB);

	//bodyUserDataA contains FALLING_PIECE
	if(bodyUserDataB->type == PhysicPiece::GROUND || bodyUserDataB->type == PhysicPiece::OLD_PIECE)
	{
		callcollision = true;
	}
}

PhysicHandler::PhysicHandler(GameOptions _gameopt):gameopt(_gameopt),  world(b2Vec2(0.0F, 15.625F)), fallingpiece(NULL), accelerating(false)
{
	b2BodyDef bodyDef;
	b2FixtureDef fixDef;
	b2PolygonShape polShape;
	PhysicPiece * piecepointer;

	piecepointer = &groundwall;
	piecepointer->type = PhysicPiece::GROUND;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(gameopt.columns/2, gameopt.rows +0.5F);
	bodyDef.userData = piecepointer;
	polShape.SetAsBox(gameopt.columns/2, 0.5F);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	piecepointer->ptr = world.CreateBody(&bodyDef);
	piecepointer->ptr->CreateFixture(&fixDef);

	piecepointer = &leftwall;
	piecepointer->type = PhysicPiece::LEFT;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(-0.5F, 0);
	bodyDef.userData = piecepointer;
	polShape.SetAsBox(0.5F, gameopt.rows);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	fixDef.friction = 0.00001;
	world.CreateBody(&bodyDef)->CreateFixture(&fixDef);

	piecepointer = &rightwall;
	piecepointer->type = PhysicPiece::RIGHT;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(gameopt.columns + 0.5F, 0);
	bodyDef.userData = piecepointer;
	polShape.SetAsBox(0.5F, gameopt.rows);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	fixDef.friction = 0.00001;
	world.CreateBody(&bodyDef)->CreateFixture(&fixDef);
	world.SetContactListener(&contactlistener);
}

PhysicPiece * PhysicHandler::createpiece(const piece<float> & pie, float x, float y, float rot, void * userdata, PhysicPiece::PhysicPieceType type, int level, float angvel, float gravscale)
{
	if (type != PhysicPiece::OLD_PIECE &&
		type != PhysicPiece::FALLING_PIECE &&
		type != PhysicPiece::NEXT_PIECE)
		return NULL;
	b2BodyDef bodyDef;
	b2FixtureDef fixDef;
	b2PolygonShape polShape;
	PhysicPiece * ret = new PhysicPiece;
	b2Body * body;

	ret->type = type;
	ret->otherdata = userdata;
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = 0.5;
	bodyDef.position.Set(x, y);
	bodyDef.angle = rot;
	bodyDef.userData = ret;
	bodyDef.angularVelocity = angvel;
	bodyDef.gravityScale = gravscale;
	body = world.CreateBody(&bodyDef);
	ret->ptr = body;

	for (const auto & pol: pie)
	{
		b2Vec2 * vertarr, * curvert;
		curvert = vertarr = new b2Vec2[pol.size()];
		for_each(pol.begin(), pol.end(), [&](point<float> p){
				curvert->Set(p.x, p.y);
				curvert++;
				});
		polShape.Set(vertarr, pol.size());
		fixDef.shape = &polShape;
		fixDef.density = 32.0F * 32.0F;
		fixDef.friction = 1.0;
		body->CreateFixture(&fixDef);
		delete [] vertarr;
	}
	if (type == PhysicPiece::FALLING_PIECE)
	{
		untagfallingpiece();
		fallingpiece = body;
		body->SetLinearVelocity(b2Vec2(0.0F, 3.125F + level*0.21875F));
	}
	return ret;
}

void PhysicHandler::untagfallingpiece()
{
	if(fallingpiece != NULL)
		static_cast<PhysicPiece*>(fallingpiece->GetUserData())->type = PhysicPiece::OLD_PIECE;
	fallingpiece = NULL;
}

void PhysicHandler::destroypiece(PhysicPiece * p)
{
	b2Body* body = p->ptr;
	if (fallingpiece == body)
		fallingpiece = NULL;
	world.DestroyBody(body);
	p->otherdata = NULL;
	delete p;
}

void PhysicHandler::piecerotate(float rot)
{
	if (fallingpiece == NULL)
		return;
	if (rot*fallingpiece->GetAngularVelocity() < 3)
		fallingpiece->ApplyTorque(rot*70*32*32, true);
}
void PhysicHandler::piecemove(float mov)
{
	if (fallingpiece == NULL)
		return;
	fallingpiece->ApplyForceToCenter(b2Vec2(mov*70*32*32,0),true);
}
void PhysicHandler::pieceaccelerate()
{
	if (fallingpiece == NULL)
		return;
	accelerating = true;
	b2Vec2 v = fallingpiece->GetLinearVelocity();
	if( v.y > 15.625)
	{
		v.y = 15.625;
		fallingpiece->SetLinearVelocity(v);
	}
	else
		fallingpiece->ApplyForceToCenter(b2Vec2(0,0.625),true);
}

void PhysicHandler::step(int level, std::function<void(float x, float y)> cb)
{
	contactlistener.callcollision = false;
	if(!accelerating && fallingpiece != NULL)
	{
		b2Vec2 v = fallingpiece->GetLinearVelocity();
		if( v.y > 3.125 + level*0.21875)
		{
			v.y -= 75 * gameopt.physicstep;
			fallingpiece->SetLinearVelocity(v);
		}
	}
	world.Step(gameopt.physicstep, 8, 3);
	accelerating = false;
	if(contactlistener.callcollision && fallingpiece != NULL)
	{
		b2Vec2 pos = fallingpiece->GetPosition();
		cb(pos.x, pos.y);
	}
	contactlistener.callcollision = false;
}

#ifndef __CHEERP__
void PhysicHandler::debugprint()
{
	for (b2Body * body = world.GetBodyList(); body; body = body->GetNext())
	{
		b2Vec2 pos = body->GetPosition();
		std::cout <<"type:" << body->GetUserData()<<"\t\tpos:"<<pos.x<<"\t"<<pos.y<<"\t\tAwake:"<<body->IsAwake()<<std::endl;
		//if(body->GetUserData() == (void*)1)
		//	body->ApplyForceToCenter(b2Vec2(0.1,0.1));
	}
	std::cout<<std::endl;
}
#endif

void PhysicHandler::iteratepieces(std::function <void (PhysicPiece *)> cb)
{
	for (b2Body * body = world.GetBodyList(); body; body = body->GetNext())
	{
		PhysicPiece * userdata = (PhysicPiece *) body->GetUserData();
		if(userdata->type != PhysicPiece::FALLING_PIECE && userdata->type != PhysicPiece::OLD_PIECE)
		if(!userdata->isfalling() && !userdata->isold() && !userdata->isnext())
			continue;
		cb(userdata);
	}

}

struct MyQueryCallback : public b2QueryCallback
{
	/* Cheerp does not support set or maps of pointers,
	 * because pointers are not globally ordered */
#ifndef __CHEERP__
	std::set<b2Body *> bodylist;
#else
	std::list<b2Body *> bodylist;
#endif

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body * body = fixture->GetBody();
#ifndef __CHEERP__
		bodylist.insert(body);
#else
		// TODO: slow
		if (find(bodylist.begin(), bodylist.end(), body) == bodylist.end())
		{
			bodylist.push_front(body);
		}
#endif
		return true;
	}
};

void PhysicHandler::getpieces_in_rect(float x0, float y0, float x1, float y1, std::function <void(PhysicPiece *)> cb)
{
	MyQueryCallback myquerycb;
	world.QueryAABB(&myquerycb, b2AABB{.lowerBound=b2Vec2(x0, y0), .upperBound=b2Vec2(x1, y1)});
	for (auto &i: myquerycb.bodylist)
	{
		PhysicPiece * php = (PhysicPiece *) i->GetUserData();
		if (!php->isold())
			continue;
		cb(php);
	}
}

void PhysicHandler::gameover()
{
	b2Body * groundbody = groundwall.ptr;
	world.DestroyBody(groundbody);
	if(fallingpiece)
	{
		PhysicPiece * userdata = (PhysicPiece *) fallingpiece->GetUserData();
		userdata->type = PhysicPiece::OLD_PIECE;
	}
	fallingpiece = NULL;
}
