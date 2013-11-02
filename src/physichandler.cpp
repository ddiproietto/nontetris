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
#ifdef __DUETTO__
#include <duetto/client.h>
#include <duetto/clientlib.h>
#include <stdio.h>
#endif
#include "physichandler.h"
#include "NontetrisConfig.h"

#ifndef __DUETTO__
#include <iostream>
#endif
#include <algorithm>

#include "polygon.h"

PhysicHandler::MyContactListener::MyContactListener():callcollision(false)
{
	
}
void PhysicHandler::MyContactListener::BeginContact(b2Contact* contact)
{
	userData * bodyUserDataA = (userData*) contact->GetFixtureA()->GetBody()->GetUserData();
	userData * bodyUserDataB = (userData*) contact->GetFixtureB()->GetBody()->GetUserData();
	bool isnA;

	if((isnA=(bodyUserDataA->type != userData::FALLING_PIECE)) && bodyUserDataB->type != userData::FALLING_PIECE)
		return;

	if(isnA)
		std::swap(bodyUserDataA,bodyUserDataB);

	//bodyUserDataA contains FALLING_PIECE
	if(bodyUserDataB->type == userData::GROUND || bodyUserDataB->type == userData::OLD_PIECE)
	{
		callcollision = true;
	}
}

PhysicHandler::PhysicHandler(float w_width, float w_height):world(b2Vec2(0.0F, 15.625F)), w_width(w_width), w_height(w_height), fallingpiece(NULL), stepInterval(1.0/60.0), accelerating(false)
{
	b2BodyDef bodyDef;
	b2FixtureDef fixDef;
	b2PolygonShape polShape;
	userData * ud;


	ud = &udground;
	ud->type = userData::GROUND;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(w_width/2, w_height +0.5F);
	bodyDef.userData = ud;
	polShape.SetAsBox(w_width/2, 0.5F);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	world.CreateBody(&bodyDef)->CreateFixture(&fixDef);

	ud = &udleft;
	ud->type = userData::LEFT;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(-0.5F, w_height/2);
	bodyDef.userData = ud;
	polShape.SetAsBox(0.5F, w_height/2);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	fixDef.friction = 0.00001;
	world.CreateBody(&bodyDef)->CreateFixture(&fixDef);

	ud = &udright;
	ud->type = userData::RIGHT;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(w_width + 0.5F, w_height/2);
	bodyDef.userData = ud;
	polShape.SetAsBox(0.5F, w_height/2);
	fixDef.shape = &polShape;
	fixDef.density = 32.0F * 32.0F;
	fixDef.friction = 0.00001;
	world.CreateBody(&bodyDef)->CreateFixture(&fixDef);
	world.SetContactListener(&contactlistener);
}

PhysicPiece * PhysicHandler::createpiece(piece<float> pie, float x, float y, float rot, void * userdata)
{
	b2BodyDef bodyDef;
	b2FixtureDef fixDef;
	b2PolygonShape polShape;
	userData * ud;
	PhysicPiece * ret = new PhysicPiece;
	b2Body * body;

	ud = &(ret->ud);
	ud->type = userData::FALLING_PIECE;
	ud->otherdata = userdata;
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = 0.5;
	bodyDef.position.Set(x, y);
	bodyDef.angle = rot;
	bodyDef.userData = ud;
	body = world.CreateBody(&bodyDef);
	ret->ptr = body;

	for (auto pol: pie)
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
	if(fallingpiece != NULL)
		((userData*)fallingpiece->GetUserData())->type = userData::OLD_PIECE;
	fallingpiece = body;
	return ret;
}

void PhysicHandler::piecerotate(float rot)
{
	if (rot*fallingpiece->GetAngularVelocity() < 3)
		fallingpiece->ApplyTorque(rot*70*32*32, true);
}
void PhysicHandler::piecemove(float mov)
{
	fallingpiece->ApplyForceToCenter(b2Vec2(mov*70*32*32,0),true);
}
void PhysicHandler::pieceaccelerate()
{
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

void PhysicHandler::step(std::function<void(float x, float y)> cb)
{
	contactlistener.callcollision = false;
	if(!accelerating)
	{
		b2Vec2 v = fallingpiece->GetLinearVelocity();
		if( v.y > 3.125)//TODO: max speed parametric to level
		{
			v.y -= 75 * stepInterval;
			fallingpiece->SetLinearVelocity(v);
		}
	}
	world.Step(stepInterval, 8, 3);
	accelerating = false;
	if(contactlistener.callcollision)
	{
		b2Vec2 pos = fallingpiece->GetPosition();
		cb(pos.x, pos.y);
	}
	contactlistener.callcollision = false;
}

#ifndef __DUETTO__
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

void PhysicHandler::drawbodies(std::function <void (float, float, float, void *)> draw)
{
	for (b2Body * body = world.GetBodyList(); body; body = body->GetNext())
	{
		userData * userdata = (userData *) body->GetUserData();
		if(userdata->type != userData::FALLING_PIECE && userdata->type != userData::OLD_PIECE)
			continue;
		b2Vec2 pos = body->GetPosition();
		float rot = body->GetAngle();
		draw(pos.x, pos.y, rot ,userdata->otherdata);

	}

}
