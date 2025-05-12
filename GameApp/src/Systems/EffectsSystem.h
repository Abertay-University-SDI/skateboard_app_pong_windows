#pragma once

#include "Skateboard/Scene/ECS.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"


class EffectsSystem final : public Skateboard::BaseSystem {
private:
	// Add member variables here!

	static inline float colourFader = 0;
	static inline int fadeType = 0;
public:
	virtual void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
};
