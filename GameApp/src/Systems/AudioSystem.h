#pragma once


#include "Skateboard/Scene/ECS.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"


class AudioSystem final : public Skateboard::BaseSystem {
private:
	// Add member variables here!


public:
	virtual void Init(entt::registry& reg) override;
	virtual void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
};
