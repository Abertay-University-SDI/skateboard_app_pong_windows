#pragma once

#include <Skateboard/Scene/Entity.h>
#include "Components/PhysicsWorldComponent.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"


class BallSystem final : public Skateboard::BaseSystem {
public:
	BallSystem(const std::shared_ptr<b2World>& world) : m_World(world)
	{
		m_Score = { 0 ,0 };
	}

	static inline uint2 GetScore() { return m_Score; };


private:
	// Add member variables here!
	
	
	std::shared_ptr<b2World> m_World;
	static inline uint2 m_Score;


	void ResetAllTempVariables(entt::registry& reg);
	void HandleCollisionWithPaddle(entt::registry& reg);
	void CheckIfBallIsOutOfRange(entt::registry& reg);
	void RestartOnButtonPress(entt::registry& reg);
public:
	virtual void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
	
};
