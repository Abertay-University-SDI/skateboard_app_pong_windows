#include "EffectsSystem.h"
#include "Components/PongComponents.h"
#include "Components/PhysicsBodyComponent.h"
#include "CameraSystem.h"
#include <Utilities/Utilities.h>
#include <Skateboard/Graphics/RHI/GraphicsContext.h>

void EffectsSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto allCollidors = reg.view<CollisionComponent, BallComponent>();

	allCollidors.each([&](CollisionComponent& collidor, BallComponent ball)
		{
			if (collidor.collidedThisFrame)
				for (auto& ent : collidor.collided_entities)
					if (ent.HasComponent<PaddleComponent>()) {
						float multiplier = (ent.GetComponent<PaddleComponent>().ballMultiplyer - 1) * 0.5;
						colourFader = multiplier;
						multiplier += 0.2f;
						CameraSystem::ShakeCamera(1.0f, 0.1*multiplier, 0.00*multiplier);
						fadeType = ent.GetComponent<PaddleComponent>().user;
					}
			if (ball.ballDiedThisFrame) {
				colourFader = 1.0f;
				fadeType = 2;
				CameraSystem::ShakeCamera(1.f, 0.02, 0.05);

			}
		});



	if (colourFader > 0)
		colourFader -= time->DeltaTime();
	else
		colourFader = 0;

	switch (fadeType)
	{
	case 0:
		// Blue Fade
		Skateboard::GraphicsContext::SetBackBufferClearColour(colourHSVtoRGBA(240.f, glm::lerp(0.f, 0.8f, colourFader), 0.3));
		break;
	case 1:
		// Green Fade
		Skateboard::GraphicsContext::SetBackBufferClearColour(colourHSVtoRGBA(glm::lerp(60.f, 120.f, colourFader), glm::lerp(0.f, 0.8f, colourFader), 0.3));
		break;

	default:
		// Red Fade
		Skateboard::GraphicsContext::SetBackBufferClearColour(colourHSVtoRGBA(0, glm::lerp(0.f, 1.f, colourFader), 0.3));
		break;
	}



}
