#pragma once
#include <Box2D/box2d.h>
#include <memory>

struct PhysicsWorldComponent
{
	std::shared_ptr<b2World> m_world;
};