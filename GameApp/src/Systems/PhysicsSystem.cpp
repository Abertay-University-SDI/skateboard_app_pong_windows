#include "sktbdpch.h"
#include "PhysicsSystem.h"
#include "Skateboard/Scene/ECS.h"

#define SKTBD_LOG_COMPONENT "PHYSICS SYSTEM"
#include <Components/CollisionResponseComponent.h>

#include "Components/PongComponents.h"
#include "Skateboard/Log.h" 

//Physics System sample Box2D

void PhysicsSystem2D::Init(entt::registry& reg)
{

}


void PhysicsSystem2D::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	//update physics World
	m_World->Step(timeStep, velocityIterations, positionIterations);

	auto physicsObjects2D = reg.view<RigidBody2D, Skateboard::TransformComponent>();

	for (auto entity : physicsObjects2D)
	{
		auto [rigidbody, transform] = physicsObjects2D.get<RigidBody2D, Skateboard::TransformComponent>(entity);

		b2Vec2 translation	= rigidbody.Body->GetPosition();
		float  Zrotation	= rigidbody.Body->GetAngle();
		

		//SKTBD_APP_TRACE("position x:{0},y:{1}", translation.x, translation.y);

		transform.m_transform.Translation = float3(translation.x, translation.y, transform.m_transform.Translation.z);
		transform.m_transform.SetEulerZ(Zrotation);
	}

	//if (m_Resolver) m_Resolver(reg,PhysWorld->GetContactList(), PhysWorld->GetContactCount());

	CheckCollisions(reg);
}

void PhysicsSystem2D::CheckCollisions(entt::registry& reg)
{
	auto contact = m_World->GetContactList();
	auto contact_count = m_World->GetContactCount();



	// Empty out the collision components for all physics objects
	auto allCollidors = reg.view<CollisionComponent, RigidBody2D>();
	allCollidors.each([&](CollisionComponent& collidor, RigidBody2D body)
		{
			collidor.collidedThisFrame = false;
			collidor.collided_entities.clear();
			collidor.collisionNormals.clear();
		});


	// Find collisions with existing objects
	for (int i = 0; i < contact_count; i++)
	{
		//Check if two bodies are touching  in the first contact
		if (contact->IsTouching())
		{
			//Get two touching bodies 
			b2Body* firstBody = contact->GetFixtureA()->GetBody();
			b2Body* secondBody = contact->GetFixtureB()->GetBody();

			//Important: Get the two entities from the touching b2Bodies
			entt::entity ent1 = (entt::entity)(firstBody->GetUserData().pointer);
			entt::entity ent2 = (entt::entity)(secondBody->GetUserData().pointer);

			if (reg.valid(ent1) && reg.valid(ent2))
			{
				entt::entity other;
				entt::entity ball;
				//figure which one isnt the ball
				if (reg.any_of<BallComponent>(ent1)) {
					ball = ent1;
					other = ent2;
				}
				else if (reg.any_of<BallComponent>(ent2)) {
					other = ent1;
					ball = ent2;
				}
				else
					continue;

				// Register all collisions
				if (reg.any_of<CollisionComponent>(ball))
				{
					auto& col = reg.get<CollisionComponent>(ball);
					col.collidedThisFrame = true;
					col.collided_entities.push_back({ other,&reg });
					col.collisionNormals.push_back({ contact->GetManifold()->localNormal.x , contact->GetManifold()->localNormal.y });
				}

			}
		}
		//Get next contact in the from the contact list
		contact = contact->GetNext();
	}

}