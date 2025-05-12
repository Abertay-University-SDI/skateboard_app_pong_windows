#include "sktbdpch.h"
#include "SpriteAnimatorSystem.h"

#include "Components/SpriteAnimComponent.h"
#include "CMP208/Renderer2D/Renderer2D.h"

#define SKTBD_LOG_COMPONENT "SPRITE ANIMATOR SYSTEM"
#include "Skateboard/Log.h" 

namespace Skateboard::Sprite
{
	void SpriteAnimatorSystem::Init(entt::registry& reg)
	{
		//nothing to init here;
	}

	void SpriteAnimatorSystem::HandleInput(Skateboard::TimeManager* time, entt::registry& reg)
	{
	}

	void SpriteAnimatorSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
	{
		auto AnimatedSprites = reg.view<SpriteAnimComponent, Sprite::SpriteComponent>();

		AnimatedSprites.each([time](const auto entity, SpriteAnimComponent& animateur, Skateboard::Sprite::SpriteComponent& animatee)
			{
				//if the track changed updated the members of the animator
				if (animateur.new_track != animateur.CurrentTrack)
				{
					animateur.CurrentTrack = animateur.new_track;

					SKTBD_ASSERT_M(animateur.texture, "Sprite animator", "Invalid Texture");

					float2 t_size;
					t_size.x = animateur.texture->GetWidth();
					t_size.y = animateur.texture->GetHeight();

					float2 framesize = animateur.Tracks[animateur.CurrentTrack].FrameSize;
					float2 offset = animateur.Tracks[animateur.CurrentTrack].StartOffset;

					animateur.UvOffset = offset / t_size;
					float2 newScale = framesize / t_size;

					animatee.m_Data.m_SpriteTextureID = animateur.texture->GetViewIndex();

					animatee.m_Data.m_UVScale = newScale;
					animatee.m_Data.m_UVOffset = animateur.UvOffset;

					animateur.ElapsedTime = 0;
				}

				//if we want to start from the beginning
				if (animateur.restart)
				{
					animateur.CurrentFrame = 0;
					animateur.restart = false;
				}

				//if we want to mirror the frame X;
				if (animateur.flipX)
				{
					animatee.m_Data.m_UVScale = { -animatee.m_Data.m_UVScale.x, animatee.m_Data.m_UVScale.y };
					animateur.flipX = false;
				}

				//mirror Y
				if (animateur.flipY)
				{
					animatee.m_Data.m_UVScale = { animatee.m_Data.m_UVScale.x, -animatee.m_Data.m_UVScale.y };
					animateur.flipY = false;
				}

				//if playing the animation, when we pass the elapsed time we want to update the animation offset in the sprite sheet
				if (animateur.playing && !animateur.paused)
				{
					animateur.ElapsedTime += time->DeltaTime();

					SpriteAnimComponent::Track track = animateur.Tracks[animateur.CurrentTrack];

					if (animateur.ElapsedTime > 1.f / animateur.Framerate)
					{
						//SKTBD_APP_INFO("animation frame: {}", animateur.CurrentFrame);

						animateur.ElapsedTime = 0;
						animatee.m_Data.m_UVOffset = animateur.UvOffset + animatee.m_Data.m_UVScale * animateur.CurrentFrame * track.FrameIncrement * ((animateur.reverse) ? -1 : 1);

						++animateur.CurrentFrame;

						if (animateur.CurrentFrame >= track.FrameCount)
						{
							if (animateur.loop)
							{
							animateur.restart = true;
							}
							else
							{
							animateur.playing = false;
							}
						}
						
					}
				}
			}
		);
	}
}
