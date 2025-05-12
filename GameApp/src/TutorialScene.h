#pragma once
#include "CMP203/Renderer203.h"
#include "CMP208/Renderer2D/Renderer2D.h"
#include "CMP208/Renderer3D/Renderer3D.h"
#include "Components/PhysicsWorldComponent.h"
#include "Skateboard/Scene/Scene.h"
#include "Skateboard/ComponentSystems/Systems/BaseSystem.h"
#include "Systems/BallSystem.h"

class TutorialScene : public Skateboard::Scene {
public:
	explicit TutorialScene(const std::string& name);

	TutorialScene() = delete;

	virtual void OnUpdate(Skateboard::TimeManager* time) override;
	virtual void OnRender() override;

	virtual void OnImGuiRender() override;
private:
	entt::registry m_Registry;

	Skateboard::Sprite::Renderer2D renderer2D;
	std::vector<std::unique_ptr<Skateboard::BaseSystem>> m_updatingSystems;

	std::shared_ptr<b2World> m_World;
	Entity m_Ball;
};
