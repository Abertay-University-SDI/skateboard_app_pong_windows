#pragma once
#include <vector>
#include <Skateboard/Scene/Entity.h>

struct PaddleComponent {
	float speed = 500.f;
	float ballMultiplyer = 1;
	int user;
};

struct BallComponent {
	float speed = 10.f;
	bool ballDied = true;
	bool ballDiedThisFrame = false;
	bool ballRespawnedThisFrame = false;
};

struct CollisionComponent {
	bool collidedThisFrame = false;
	std::vector<Skateboard::Entity> collided_entities;
	std::vector<float2> collisionNormals;
};