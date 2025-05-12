#include "CameraSystem.h"
#include "Skateboard/Scene/Components.h"
#include "Skateboard/Scene/Scene.h"
#include "Skateboard/Input.h"

void CameraSystem::ShakeCamera(float time, float tmag, float rmag)
{
	if (mainCamera) {
		if (mainCamera.HasComponent<CameraShakeComponent>())
		{
			auto& camShke = mainCamera.GetComponent<CameraShakeComponent>();
			camShke.shake_time_ = time;
			camShke.trans_magnitude_ = tmag;
			camShke.rot_magnitude_ = rmag;
		}

	}
}

void CameraSystem::HandleInput(Skateboard::TimeManager* time, entt::registry& reg)
{

	// Control camera from an input component


}


void CameraSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& registry)
{
	

	auto view = registry.view<Skateboard::TransformComponent, Skateboard::CameraComponent>();
	for (auto entity : view) {
		auto [transform, camera] = view.get<Skateboard::TransformComponent, Skateboard::CameraComponent>(entity);

		if (camera.Primary)
		{
			// If the Camera is the Main Camera

			// Check if the camera is a perspective camera by using Dynamic Cast https://www.geeksforgeeks.org/dynamic-_cast-in-cpp/
			Skateboard::PerspectiveCamera* perspCam = dynamic_cast<Skateboard::PerspectiveCamera*>(camera.Camera.get());

			// If the current camera is of the type perspective camera then:
			if (perspCam)
			{
			}
			//else if (OrthogrpahicCamera)
				// Update Orthographic camera ...

			// Update the camera view matrix  based on the transform component of the entity
			if(!mainCamera)
			mainCamera = { entity ,&registry};
			Transform tempTransform = transform.m_transform;
			if (mainCamera.HasComponent<CameraShakeComponent>())
			{
				auto& camShke = mainCamera.GetComponent<CameraShakeComponent>();
				tempTransform = UpdateShake(transform.m_transform, camShke, time->DeltaTime());
			}
			// NOTE: Without this, the camera position will not be updated in the renderer
			camera.Camera->UpdateViewMatrix(tempTransform);
		}
	}

}

#define SHAKESPEEDX 40.0f
#define SHAKESPEEDY 30.0f
#define SHAKESPEEDZ 32.0f

Transform CameraSystem::UpdateShake(Transform& trans, CameraShakeComponent& camShake, float deltaTime)
{
	Transform tempTransform = trans;
	

	

	if (camShake.shake_time_ >= 0.f) {


		// Check if the camera shake is active

		sax += SHAKESPEEDX * deltaTime;
		say += SHAKESPEEDY * deltaTime;
		saz += SHAKESPEEDZ * deltaTime;

		float shakex = camShake.shake_time_ * sinf(sax);
		float shakey = camShake.shake_time_ * sinf(say);
		float shakez = camShake.shake_time_ * sinf(saz);

		if (camShake.shake_time_ >= 0.f)

			camShake.shake_time_ -= deltaTime; // Reduce the shake time every frame it is active
		// ensure that the constants are multiplied with deltaTime for smoothing
		// these three (sax, say, saz) are member variables of your Camera system

		// Reduce the magnitude of the shake as it progresses. High impact which reduces over time
		// Sinf here ensures that you get a random number between -1 & +1;
		// sax, say & saz are incremented every frame

		float3 lookat, rot;
		lookat = trans.Translation + trans.GetForwardVector(); // Get the current transform lookat

		lookat.x += camShake.trans_magnitude_ * shakex;
		lookat.y += camShake.trans_magnitude_ * shakey;
		lookat.z += camShake.trans_magnitude_ * shakez;

		// Move this slightly based on the calculated values -- this can be heavy or slight

		rot = trans.AsEuler();

		rot.x += camShake.rot_magnitude_ * shakex;
		rot.y += camShake.rot_magnitude_ * shakey;
		rot.z += camShake.rot_magnitude_ * shakez;

		tempTransform.Translation += float3(shakex,shakey,shakez) * camShake.trans_magnitude_;
		tempTransform.LookAt(lookat);
		tempTransform.SetEulerAngles(rot.x, rot.y, rot.z); // want to lose our existing camera target, this temporary transform is just an offset
	}
	return tempTransform;

	// Get the current rotation values

	// Change them slightly for the camera shake based on the calculated shake parameters

	// Note that we are changing the temporary transform here -- we do not
}
