#pragma once
#include "Skateboard/Scene/Components.h"


struct CollisionResponseComponent
{
	CollisionResponseComponent(float other=0, float self=0) : selfHealthToReduce(self), otherHealthToReduce(other){};

	float selfHealthToReduce;
	float otherHealthToReduce;
};

struct HealthComponent {
	HealthComponent(float h) : health(h) {};
	float health = 100;
};
