#pragma once
#include "Skateboard/Scene/Entity.h"
#include "Box2D/box2d.h"

#define PADDLE_HEIGHT 6

class EntityBuilder {
public:
	
	static std::shared_ptr<b2World> CreatePhysicsWorld(entt::registry& reg);
	static void CreatePaddle(entt::registry& reg, std::shared_ptr<b2World> world, bool left = false);
	static void CreateWall(entt::registry& reg, std::shared_ptr<b2World> world, bool up = false);
	static void CreateBall(entt::registry& reg, std::shared_ptr<b2World> world);
	static void CreateCamera(entt::registry& reg);
	
};