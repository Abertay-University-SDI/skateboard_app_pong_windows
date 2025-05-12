// includes all the basic structures and most standard library containers used throughout Skateboard
#include "sktbdpch.h"
#include "TutorialScene.h"

#include "Components/PongComponents.h"
#include "ECS/EntityBuilder.h"
#include "Systems/CameraSystem.h"
#include "Systems/PaddleSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/EffectsSystem.h"
#include "Systems/RechargeSystem.h"
#include "Systems/BallEffects.h"
#include "Systems/AudioSystem.h"
#include "Systems/SpriteAnimatorSystem.h"
#include "Skateboard/Scene/SceneBuilder.h"
#include <Skateboard/Platform.h>
#include "Systems/BallSystem.h"


TutorialScene::TutorialScene(const std::string& name): 
	Scene(name)
{
	renderer2D.Init();
	renderer2D.Register(&m_Registry, "tutorialSCN"); 

	AssetManager::LoadFont(L"assets/fonts/Coiny-Regular.ttf", 60, "Coiny");

	m_World = EntityBuilder::CreatePhysicsWorld(m_Registry);
	EntityBuilder::CreateCamera(m_Registry);
	EntityBuilder::CreatePaddle(m_Registry, m_World);
	EntityBuilder::CreatePaddle(m_Registry, m_World, true);
	EntityBuilder::CreateWall(m_Registry, m_World);
	EntityBuilder::CreateWall(m_Registry, m_World, true);
	EntityBuilder::CreateBall(m_Registry, m_World);


	//after creating the world we can push the physics system onto the update system vector
	m_updatingSystems.push_back(std::make_unique<PhysicsSystem2D>(m_World));
	m_updatingSystems.push_back(std::make_unique<CameraSystem>());
	//m_updatingSystems.push_back(std::make_unique<PaddleSystem>());
	//m_updatingSystems.push_back(std::make_unique<RechargeSystem>());
	//m_updatingSystems.push_back(std::make_unique<EffectsSystem>());
	//m_updatingSystems.push_back(std::make_unique<BallEffects>());
	//m_updatingSystems.push_back(std::make_unique<AudioSystem>());
	m_updatingSystems.push_back(std::make_unique<BallSystem>(m_World));

	


	for (auto& s : m_updatingSystems)
		s->Init(m_Registry);
} 
 

void TutorialScene::OnUpdate(Skateboard::TimeManager* time)
{
	Scene::OnUpdate(time);

	

	// Loop through all systems and call run on it
	for (auto& s : m_updatingSystems)
		s->RunUpdate(time, m_Registry);
}

void TutorialScene::OnRender()
{
	renderer2D.Begin();
	renderer2D.Update();
	renderer2D.End();

}

void TutorialScene::OnImGuiRender()
{
	auto mainWinPos = ImGui::GetWindowPos();
	ImGui::Begin("ImGui", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);//creates new window
	ImGui::SetWindowSize({ 200,100 });
	auto scr_width = Skateboard::Platform::GetGraphicsContext()->GetClientWidth();
	ImGui::SetWindowPos({mainWinPos.x+(scr_width /2)-75 , mainWinPos.y +30});
	//ImGui::Begin("ImGui");//creates new window
	ImGui::PushFont(AssetManager::GetFont("Coiny"));
#if SKTBD_PLATFORM_WINDOWS
	ImGui::SetWindowFontScale((float)scr_width / 1080.0f * 0.3f);
#endif

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::Text("%i | %i", BallSystem::GetScore().x, BallSystem::GetScore().y);
	ImGui::PopStyleVar();

	ImGui::PopFont();

	////how to use if defs
	//#ifdef SKTBD_PLATFORM_PLAYSTATION
	//	ImGui::Text("Hello Prospero!");
	//#elif SKTBD_PLATFORM_WINDOWS
	//	ImGui::Text("Hello Windows!");
	//#endif
	ImGui::End();
}
