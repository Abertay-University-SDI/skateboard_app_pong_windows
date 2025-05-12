#include "AudioSystem.h"
#include <Skateboard/Audio/AudioRenderer.h>
#include "Components/PongComponents.h"
#include "Skateboard/Platform.h"
#include "Components/PhysicsBodyComponent.h"

using namespace Skateboard;
void AudioSystem::Init(entt::registry& reg)
{
#ifdef SKTBD_PLATFORM_PLAYSTATION
	Skateboard::AudioRenderer::InitPlatform();
	Skateboard::AudioRenderer::LoadBank("assets/audio/PongBank.bnk", "pongBank");
	AudioRenderer::AddLocalPlayer({ Platform::GetUserManager()->GetDefaultUser().id });

#elif SKTBD_PLATFORM_WINDOWS
	Skateboard::AudioRenderer::InitPlatform();
	Skateboard::AudioRenderer::LoadAudio("assets/audio/PaddleHit0.wav", "PaddleHit0");
	Skateboard::AudioRenderer::LoadAudio("assets/audio/PaddleHit1.wav", "PaddleHit1");
	Skateboard::AudioRenderer::LoadAudio("assets/audio/BallLost.wav", "BallLost");
	Skateboard::AudioRenderer::LoadAudio("assets/audio/BallStarted.wav", "BallStarted");

	AudioRenderer::AddLocalPlayer({ 0 });
#endif // SKTBD_PLATFORM_PLAYSTATION

	Skateboard::AudioRenderer::PlayAudio("BallLost");
}

void AudioSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto view = reg.view<CollisionComponent, BallComponent>();

	view.each([&](CollisionComponent& collidor, BallComponent ball)
		{
			if (collidor.collidedThisFrame)
				for (auto& ent : collidor.collided_entities) 
					if (ent.HasComponent<PaddleComponent>()) {
						auto& pad = ent.GetComponent<PaddleComponent>();
						if (pad.ballMultiplyer > 0.0f) 
						{
							if (pad.user == 0)
								AudioRenderer::PlayAudio("PaddleHit0");
							else
								AudioRenderer::PlayAudio("PaddleHit1");
						}
					}

			if (ball.ballDiedThisFrame)
				AudioRenderer::PlayAudio("BallLost");
			if(ball.ballRespawnedThisFrame)
				AudioRenderer::PlayAudio("BallStarted");

		});


}
