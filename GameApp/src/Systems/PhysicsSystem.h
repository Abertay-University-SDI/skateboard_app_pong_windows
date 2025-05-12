#pragma once

#include "box2d/box2d.h"
#include "Skateboard/Scene/ECS.h"
#include "Components/PhysicsBodyComponent.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"
#include <functional>

//FOR this to work project must target project must link against Box2D
class PhysicsSystem2D : public Skateboard::BaseSystem
{
public:
    PhysicsSystem2D(const std::shared_ptr<b2World>& world) : m_World(world) {};
    PhysicsSystem2D() : m_World() {};
    
    void Init(entt::registry& reg) override;

    void RunUpdate(Skateboard::TimeManager* time, entt::registry& reg) override;

	//void SetCollisionResolver(const std::function<void(entt::registry& reg, b2Contact* contact_list, uint32_t contact_count)>& function) { m_Resolver = function; };

private:
    //std::function<void(entt::registry& reg, b2Contact* contact_list, uint32_t contact_count)> m_Resolver;

    void CheckCollisions(entt::registry& registry);

    std::shared_ptr<b2World> m_World;

    const float timeStep = 1.f / 60.f;
    const int32 velocityIterations = 8;
    const int32 positionIterations = 3;
};



