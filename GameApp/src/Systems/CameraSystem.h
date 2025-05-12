#pragma once
#include "Skateboard/Scene/ECS.h"
#include "Skateboard/Camera/CameraController.h"
#include "Skateboard/Scene/Entity.h"
#include "Components/CameraShakeComponent.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"


class CameraSystem final : public Skateboard::BaseSystem {
private:
	// Add member variables here!
	static inline Skateboard::Entity mainCamera;
	float m_speed = 5.f;

	float sax;
	float say;
	float saz;
	Transform UpdateShake(Transform& trans, CameraShakeComponent& camShake, float deltaTime);


public:

	static void ShakeCamera(float time, float tmag, float rmag);
	virtual void HandleInput(Skateboard::TimeManager* time, entt::registry& reg) override;
	virtual void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
};
