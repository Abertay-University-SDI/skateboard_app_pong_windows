#pragma once

#include "Skateboard/Scene/ECS.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"


class BallEffects final : public Skateboard::BaseSystem {
private:
	// Add member variables here!

	inline static float sizeSpeed = 5.0f;
public:
	virtual void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
};
