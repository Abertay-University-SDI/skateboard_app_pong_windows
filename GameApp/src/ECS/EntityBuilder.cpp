#include "EntityBuilder.h"
#include "Skateboard/Scene/ECS.h"
#include "Skateboard/Graphics/RHI/GraphicsContext.h"
#include "Skateboard/Scene/Components.h"
#include "CMP208/Renderer2D/Renderer2D.h"
#include "Skateboard/Assets/AssetManager.h"
#include "Components/PhysicsWorldComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/PongComponents.h"
#include "Components/CameraShakeComponent.h"



using namespace Skateboard;

std::shared_ptr<b2World> EntityBuilder::CreatePhysicsWorld(entt::registry& reg)
{
	auto physics_world_ent = ECS::CreateEntity("physics_world", reg);
	auto& pw = physics_world_ent.AddComponent<PhysicsWorldComponent>();
	//define gravity
	b2Vec2 gravity(0, 0);
	//create world
	pw.m_world = std::make_shared<b2World>(gravity);
	return pw.m_world;

}

void EntityBuilder::CreatePaddle(entt::registry& reg, std::shared_ptr<b2World> world, bool left)
{
	auto paddle = ECS::CreateEntity("Paddle", reg);

	auto& transform = paddle.AddComponent<TransformComponent>().m_transform;
	auto& sprite = paddle.AddComponent<Sprite::SpriteComponent>().m_Data;
	auto& padComponent = paddle.AddComponent<PaddleComponent>();
	padComponent.user = int(left);


	sprite.m_Colour.r = 1;
	sprite.m_Colour.g = 1;
	sprite.m_Colour.b = 1;
	sprite.m_Colour.a = 1;
	sprite.m_SpriteTextureID = AssetManager::GetDefaultTexture(TextureDimension_Texture2D)->GetViewIndex();
	sprite.m_Size = { 1.5,PADDLE_HEIGHT };
	sprite.m_Offset = { sprite.m_Size.x / -2.0, sprite.m_Size.y / 2.0 };

	int direction = 1;
	if (left)
		direction = -1;

	transform.Translation.x = 17 * direction;
	transform.Translation.y = 0;

	//transform.Scale = float3(3);

	//create dynamic body
	b2BodyDef bdef;
	bdef.type = b2_dynamicBody;
	bdef.position.x = transform.Translation.x;
	bdef.position.y = transform.Translation.y;
	bdef.fixedRotation = true;
	//bdef.angularVelocity = 5.f;
	bdef.awake = true;
	bdef.userData.pointer = (uintptr_t)paddle.GetEnttHandle();
	b2Body* body = world->CreateBody(&bdef);

	//now we need to define a shape and attach it to a body with a fixture

	b2PolygonShape staticBox;
	staticBox.SetAsBox(sprite.m_Size.x / 2.0, sprite.m_Size.y / 2.0); // half size ,mind the pivot is in the center

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &staticBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);
	paddle.AddComponent<RigidBody2D>(body);

	
}

void EntityBuilder::CreateWall(entt::registry& reg, std::shared_ptr<b2World> world, bool up)
{
	auto entity = ECS::CreateEntity("Paddle", reg);

	auto& transform = entity.AddComponent<TransformComponent>().m_transform;
	auto& sprite = entity.AddComponent<Sprite::SpriteComponent>().m_Data;
	//auto& padComponent = paddle.AddComponent<PaddleComponent>();
	//padComponent.user = int(up);


	sprite.m_Colour.r = 1;
	sprite.m_Colour.g = 1;
	sprite.m_Colour.b = 1;
	sprite.m_Colour.a = 1;
	sprite.m_SpriteTextureID = AssetManager::GetDefaultTexture(TextureDimension_Texture2D)->GetViewIndex();
	sprite.m_Size = { 40,1.5 };
	sprite.m_Offset = { sprite.m_Size.x / -2.0, sprite.m_Size.y / 2.0 };

	int direction = 1;
	if (up)
		direction = -1;

	transform.Translation.x = 0;
	transform.Translation.y = 10.25 * direction;

	//transform.Scale = float3(3);

	//create dynamic body
	b2BodyDef bdef;
	bdef.type = b2_staticBody;
	bdef.position.x = transform.Translation.x;
	bdef.position.y = transform.Translation.y;
	//bdef.angularVelocity = 5.f;
	bdef.awake = true;
	bdef.userData.pointer = (uintptr_t)entity.GetEnttHandle();
	b2Body* body = world->CreateBody(&bdef);

	//now we need to define a shape and attach it to a body with a fixture

	b2PolygonShape staticBox;
	staticBox.SetAsBox(sprite.m_Size.x/2.0 , sprite.m_Size.y/2.0); // half size ,mind the pivot is in the center

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &staticBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);
	entity.AddComponent<RigidBody2D>(body);


}

void EntityBuilder::CreateBall(entt::registry& reg, std::shared_ptr<b2World> world)
{
	auto entity = ECS::CreateEntity("Ball", reg);

	auto& transform = entity.AddComponent<TransformComponent>().m_transform;
	auto& sprite = entity.AddComponent<Sprite::SpriteComponent>().m_Data;
	auto& ballcomp = entity.AddComponent<BallComponent>();
	auto& ballCollision = entity.AddComponent<CollisionComponent>();
	ballcomp.speed = 5.0f;

	sprite.m_Colour.r = 1;
	sprite.m_Colour.g = 1;
	sprite.m_Colour.b = 1;
	sprite.m_Colour.a = -1;
	sprite.m_SpriteTextureID = AssetManager::GetDefaultTexture(TextureDimension_Texture2D)->GetViewIndex();
	sprite.m_Size = { 1,1 };
	sprite.m_Offset = { -.5, .5 };

	transform.Translation.x = 0;
	transform.Translation.y = 0;

	//transform.Scale = float3(3);

	//create dynamic body
	b2BodyDef bdef;
	bdef.type = b2_dynamicBody;
	bdef.position.x = transform.Translation.x;
	bdef.position.y = transform.Translation.y;
	bdef.awake = true;
	bdef.userData.pointer = (uintptr_t)entity.GetEnttHandle();
	b2Body* body = world->CreateBody(&bdef);

	//now we need to define a shape and attach it to a body with a fixture

	

	b2CircleShape ball({});

	ball.m_radius = sprite.m_Size.y / 2.0; // half size ,mind the pivot is in the center

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &ball;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0;
	fixtureDef.restitution = 1.f;
	fixtureDef.restitutionThreshold = 0.f;

	body->CreateFixture(&fixtureDef);
	entity.AddComponent<RigidBody2D>(body);

}

void EntityBuilder::CreateCamera(entt::registry& reg)
{
	auto cameraEnt = ECS::CreateEntity("camera", reg);
	auto& camTrans = cameraEnt.AddComponent<TransformComponent>().m_transform;
	auto& cameraComp = cameraEnt.AddComponent<CameraComponent>();
	cameraEnt.AddComponent<CameraShakeComponent>();

	// Setting the position and LookAt values of the camera.
	camTrans.Translation = float3(0, 0, -10);
	camTrans.LookAt(float3(0, 0, 0));			// Lookat function just adjusts the rotation based on a target (press F12 on the function to peek in)

	cameraComp.Primary = true;
	// Create an Perspective / Orthographic Camera Object and store it in your Camera Component pointer.
	auto aspect = GraphicsContext::GetClientAspectRatio();
	//cameraComp.Camera = std::make_unique<OrthographicCamera>(16, 9, 0.001, 100);
	cameraComp.Camera = std::make_unique<PerspectiveCamera>(glm::radians(90.0f), aspect, 0.01f, 1000.f);

}
