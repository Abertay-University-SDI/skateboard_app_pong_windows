#include "BallSystem.h"

#include "Components/PhysicsBodyComponent.h"
#include "Systems/CameraSystem.h"
#include "Components/PongComponents.h"
#include "ECS/EntityBuilder.h"


void BallSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{

	ResetAllTempVariables(reg);

	HandleCollisionWithPaddle(reg);

	CheckIfBallIsOutOfRange(reg);

	RestartOnButtonPress(reg);

}



void BallSystem::ResetAllTempVariables(entt::registry& reg)
{
	auto balls = reg.view<BallComponent>();

	balls.each([&](BallComponent& ball)
		{
			ball.ballDiedThisFrame = false;
			ball.ballRespawnedThisFrame = false;
		});
}

void BallSystem::HandleCollisionWithPaddle(entt::registry& reg)
{
	// Loop through all collisions to look for ball (only ball has a collision component at the moment)
	auto view = reg.view<CollisionComponent, RigidBody2D>();

	for (auto ball : view)
	{
		auto [collider, body] = view.get<CollisionComponent, RigidBody2D>(ball);
		int index = 0;
		if (collider.collidedThisFrame)
			for (auto& other : collider.collided_entities)
			{
				//collision with the paddle
				if (other.HasComponent<PaddleComponent>())
				{
					auto& pad = other.GetComponent<PaddleComponent>();
					auto& t1 = reg.get<Skateboard::TransformComponent>(ball);
					auto& t2 = reg.get<Skateboard::TransformComponent>(other);

					auto Y_intersect = t1.m_transform.Translation - t2.m_transform.Translation;

					auto norm_Y_Intersect = Y_intersect.y / (PADDLE_HEIGHT / 2);

					auto const MAXBOUNCEANGLE = glm::radians(60.f);

					auto bounceAngle = norm_Y_Intersect * MAXBOUNCEANGLE;

					auto const BALLSPEED = reg.get<BallComponent>(ball).speed * pad.ballMultiplyer;
					pad.ballMultiplyer = 1;

					b2Vec2 ballV;

					ballV.x = BALLSPEED * glm::cos(bounceAngle) * glm::sign(collider.collisionNormals[index].x);
					ballV.y = BALLSPEED * glm::sin(bounceAngle);

					reg.get<RigidBody2D>(ball).Body->SetLinearVelocity(ballV);


				}
				else //collision with a wall
				{
					//do nothing
				}

				index++;
			}
	}
}

void BallSystem::CheckIfBallIsOutOfRange(entt::registry& reg)
{
	// Loop through all the ball components and check if they are out of scope
	auto balls = reg.view<BallComponent, RigidBody2D>();

	balls.each([&](BallComponent& ball, RigidBody2D body)
		{
			auto& body_p = body.Body->GetTransform().p;

			// This means that the ball is out of the screen range. Time to reset everything and add to score values
			if (abs(body_p.x) > 17)
			{
				if (body_p.x > 0.f)
					m_Score.x += 1;
				else
					m_Score.y += 1;
				
				ball.ballDied = true;
				ball.ballDiedThisFrame = true;
				body.Body->SetLinearVelocity({ 0,0 });
				body.Body->SetTransform({ 0,0 }, 0);
			}
		});

}

void BallSystem::RestartOnButtonPress(entt::registry& reg)
{

	// On Button Press
	if (Skateboard::Input::IsButtonPressed(Skateboard::GamePadButton_::Pad_Button_Bottom) || Skateboard::Input::IsKeyPressed(Skateboard::Keys::sc_space))
	{
		auto view = reg.view<BallComponent, RigidBody2D>();

		view.each([](BallComponent& ball, RigidBody2D& body)
			{
				if (ball.ballDied) 
				{
					ball.ballDied = false;
					ball.ballRespawnedThisFrame = true;
					float direction = rand() % 2 ? -1.f : 1.f;
					body.Body->SetLinearVelocity({ ball.speed * direction,0 });

				}
			});
	}
}


