#include "BallEffects.h"
#include "Components/PongComponents.h"
#include "Components/PhysicsBodyComponent.h"
#include "Skateboard/Scene/Components.h"

void BallEffects::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto view = reg.view<CollisionComponent, BallComponent, Skateboard::TransformComponent>();

	view.each([&](CollisionComponent& collidor, BallComponent ball, Skateboard::TransformComponent& trans)
		{
			trans.m_transform.Scale += float3(1) * time->DeltaTime()*sizeSpeed;
			trans.m_transform.Scale = glm::clamp(trans.m_transform.Scale, float3(0), float3(1));
			
			if(collidor.collidedThisFrame)
				for (auto& ent : collidor.collided_entities)
					if (ent.HasComponent<PaddleComponent>()) {
						float multiplier = (ent.GetComponent<PaddleComponent>().ballMultiplyer - 1) * 0.5;
						trans.m_transform.Scale *= (1 - multiplier*0.3f)*0.75f;
						sizeSpeed = 0.8;
					}

			if (ball.ballDied) {
				trans.m_transform.Scale = float3(0);
				sizeSpeed = 5;
			}
				
		});


}
