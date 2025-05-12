#pragma once

#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"

namespace Skateboard::Sprite
{
    class SpriteAnimatorSystem :
        public Skateboard::BaseSystem
    {
        void Init(entt::registry& reg) override;

        void HandleInput(Skateboard::TimeManager* time, entt::registry& reg) override;
        void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;
    };
}
