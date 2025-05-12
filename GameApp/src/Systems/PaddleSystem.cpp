#include "PaddleSystem.h"
#include "Skateboard/Scene/Components.h"
#include "Skateboard/Scene/Scene.h"
#include "Components/PongComponents.h"
#include "Components/PhysicsBodyComponent.h"
#include "Skateboard/Input.h"

void PaddleSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto view = reg.view<RigidBody2D, PaddleComponent>();
	for (auto entity : view) 
	{
		auto [phy, pad] = view.get<RigidBody2D, PaddleComponent>(entity);
	

		// initialize velocity to zero
		phy.Body->SetLinearVelocity({ 0,0});



		if (pad.user == 0) 
		{
			// Keyboard Input
			if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_arrowUp))
				phy.Body->SetLinearVelocity({ 0,pad.speed * time->DeltaTime() });
			if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_arrowDown))
				phy.Body->SetLinearVelocity({ 0,-pad.speed * time->DeltaTime() });
		}
		else {

			// Keyboard Input
			if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_w))
				phy.Body->SetLinearVelocity({ 0,pad.speed * time->DeltaTime() });
			if (Skateboard::Input::IsKeyDown(Skateboard::Keys::sc_s))
				phy.Body->SetLinearVelocity({ 0,-pad.speed * time->DeltaTime() });
		}
			


		

	}
}
