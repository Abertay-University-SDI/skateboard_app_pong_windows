#include "RechargeSystem.h"
#include <Skateboard/Mathematics.h>
#include <Skateboard/Input.h>
#include <Components/PongComponents.h>
#include <Components/PhysicsBodyComponent.h>

void RechargeSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto view = reg.view<Skateboard::TransformComponent, PaddleComponent>();
	for (auto entity : view)
	{
		auto [trans, pad] = view.get<Skateboard::TransformComponent, PaddleComponent>(entity);
		float trigger = 0;
		if (pad.user == 0)
			trigger = Skateboard::Input::GetRightTrigger();
		else
			trigger = Skateboard::Input::GetLeftTrigger();

		if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_arrowLeft) && pad.user == 0)
			trigger = 1;
		if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_d) && pad.user == 1)
			trigger = 1;

		if (glm::abs(trigger) < 0.2f)
			pad.ballMultiplyer -= time->DeltaTime();
		else {
			pad.ballMultiplyer += time->DeltaTime()*0.5f * trigger;
		}

		pad.ballMultiplyer = glm::clamp(pad.ballMultiplyer, 1.f, 3.f);
		trans.m_transform.Scale = glm::lerp(float3(1,1,1),glm::linearRand(float3(0.8),float3(1.2)), (pad.ballMultiplyer - 1.f) / 2.f);
	}
	
}
