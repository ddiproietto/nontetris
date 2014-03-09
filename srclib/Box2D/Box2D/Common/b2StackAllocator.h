/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_STACK_ALLOCATOR_H
#define B2_STACK_ALLOCATOR_H

#include <Box2D/Common/b2Settings.h>

#ifdef __DUETTO__
#include <Box2D/Dynamics/Contacts/b2ContactSolver.h>
#include <Box2D/Dynamics/b2TimeStep.h>

class b2ContactPositionConstraint;
class b2ContactVelocityConstraint;
class b2Body;
class b2Contact;
class b2Joint;
struct b2Velocity;
struct b2Position;
#endif

#ifdef __DUETTO__
const int32 b2_stackSize = 100;
#else
const int32 b2_stackSize = 100 * 1024;	// 100k
#endif
const int32 b2_maxStackEntries = 32;

struct b2StackEntry
{
	char* data;
	int32 size;
	bool usedMalloc;
};

// This is a stack allocator used for fast per step allocations.
// You must nest allocate/free pairs. The code will assert
// if you try to interleave multiple allocate/free pairs.
class b2StackAllocator
{
public:
	b2StackAllocator();
	~b2StackAllocator();

#ifdef __DUETTO__

#define DEFINE_TYPE(a,b) a * Allocate_ ## b (int32 num);
#include <Box2D/Common/b2StackAllocatorTypesDuetto.h>
#undef DEFINE_TYPE

#define DEFINE_TYPE(a,b) void Free_ ## b (a * p);
#include <Box2D/Common/b2StackAllocatorTypesDuetto.h>
#undef DEFINE_TYPE

#else
	void* Allocate(int32 size);
	void Free(void* p);
#endif

	int32 GetMaxAllocation() const;

private:

#ifdef __DUETTO__

// TODO the size is fixed and there is no allocation

#define DEFINE_TYPE(a,b) a m_data_ ## b[b2_stackSize/sizeof(a)];
#include <Box2D/Common/b2StackAllocatorTypesDuetto.h>
#undef DEFINE_TYPE

#define DEFINE_TYPE(a,b) a * pointer_ ## b;
#include <Box2D/Common/b2StackAllocatorTypesDuetto.h>
#undef DEFINE_TYPE

#else
	char m_data[b2_stackSize];
	int32 m_index;
	int32 m_allocation;

	b2StackEntry m_entries[b2_maxStackEntries];
	int32 m_entryCount;
#endif

	int32 m_maxAllocation;

};

#endif
