#include "HapticsSystem.h"
#include <Skateboard/Mathematics.h>
#include <Skateboard/Input.h>
#include <Components/PongComponents.h>
#include <Components/PhysicsBodyComponent.h>
using namespace Skateboard;
void HapticsSystem::RunUpdate(Skateboard::TimeManager* time, entt::registry& reg)
{
	auto view = reg.view<Skateboard::TransformComponent, PaddleComponent>();
	for (auto entity : view)
	{
		auto [trans, pad] = view.get<Skateboard::TransformComponent, PaddleComponent>(entity);
		
		float multiplier = (pad.ballMultiplyer - 1) * 0.5;


		SKTBDeviceHapticSettings vibSetting;
		vibSetting.mode = SKTBDPadTriggerEffectMode::SKTBD_PAD_TRIGGER_EFFECT_MODE_MULTIPLE_POSITION_VIBRATION;
		
		vibSetting.side = pad.user == 0? Side_::Right: Side_::Left;

		
		vibSetting.commandData.vibrationParam.amplitude = 8 *  multiplier;
		vibSetting.commandData.vibrationParam.frequency = 200 * multiplier;
		vibSetting.commandData.vibrationParam.position = glm::lerp(9.f,0.f,multiplier);
		
		SKTBDeviceHapticSettings slopeSetting;
		slopeSetting.side = pad.user == 0 ? Side_::Right : Side_::Left;

		slopeSetting.mode = SKTBD_PAD_TRIGGER_EFFECT_MODE_SLOPE_FEEDBACK;
		slopeSetting.commandData.slopeFeedbackParam.startPosition = 0;
		slopeSetting.commandData.slopeFeedbackParam.endPosition = 7;
		slopeSetting.commandData.slopeFeedbackParam.startStrength = 6;
		slopeSetting.commandData.slopeFeedbackParam.endStrength = 8;


		float triggerVal = pad.user == 0 ? Input::GetLeftTrigger() : Input::GetRightTrigger();
		if(triggerVal>0.8f)
			Input::SetHapticResponse(vibSetting);
		else
			Input::SetHapticResponse(slopeSetting);

	}
}
