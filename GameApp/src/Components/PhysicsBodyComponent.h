#pragma once
#include "Skateboard/Scene/Components.h"
//all you need for box2d
#include "box2d/box2d.h"

template <class PBody>
struct RigidBodyComponent
{
	RigidBodyComponent(PBody* body) : Body(body){};
	PBody* Body;
};

struct RigidBody2D : RigidBodyComponent<b2Body>
{
	RigidBody2D(b2Body* body) : RigidBodyComponent<b2Body>(body) {};
};
