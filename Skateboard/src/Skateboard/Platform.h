#pragma once
#include "sktbdpch.h"

#include "Skateboard/Graphics/RHI/GraphicsContext.h"
#include "Skateboard/Time/TimeManager.h"
#include "Skateboard/Events/Event.h"
#include "Skateboard/User.h"
#include "Skateboard/Input/DeviceManager.h"

#include "Input.h"
#include "Assets/AssetManager.h"

namespace Skateboard
{
	namespace Log
	{
		class Logger;
	}

	struct PlatformProperties
	{
		std::wstring Title = L"Skateboard Engine";

		uint32_t BackBufferWidth = 1280u;
		uint32_t BackBufferHeight = 720u;

//		Skateboard::PlatformProperties props = {};
		//uint32_t		BackBufferWidth = 1920;
		//uint32_t		BackBufferHeight = 1080;

		//props.BackBufferWidth = 3840;
		//props.BackBufferHeight = 2160;
//		props.Title = L"Playstation - Skateboard Engine";

		//
		//  BackBufferFormat = DataFormat_R8G8B8A8_UNORM;
		//DataFormat_ DepthStencilBufferFormat = DataFormat_D24_UNORM_S8_UINT;
	};

	class Platform
	{
		DISABLE_COPY_AND_MOVE(Platform)
		friend class Application;

	public:
		Platform() {};
		virtual ~Platform() {}
		// look at https://refactoring.guru/design-patterns/singleton
		static Platform& GetPlatform();

		static TimeManager* GetTimeManager() { return GetPlatform().m_Timer.get(); }
		static UserManager* GetUserManager() { return GetPlatform().m_Users.get(); }
		static DeviceManager* GetDeviceManager() { return GetPlatform().m_Devices.get(); }
		static GraphicsContext* GetGraphicsContext() { return GetPlatform().p_GraphicsContext.get(); }
		static AssetManager* GetAssetManager() { return GetPlatform().m_AssetManager.get(); }
		static Log::Logger* GetLog() { return GetPlatform().m_Logger.get(); }

		inline static std::optional<User> m_GamePadImGuiControl = std::optional<User>(); // to navigate imgui with a controller // Works only on PS5 but could be extended using new Imgui Navigation features 

		virtual void Init(const PlatformProperties& props = PlatformProperties())
		{
			Input::RegisterDeviceManager(GetDeviceManager());
		}

		virtual bool Update()
		{
			m_Timer->Tick();
			m_Users->Update();
			m_Devices->Update();
			return true;
		}

		//maybe restrict access to these
		static  void PlatformDispatchEvent(Event& e) { Platform::GetPlatform().EventCallback(e); }

		virtual void InitImGui() = 0;
		virtual void BeginImGuiPass() = 0;
		virtual void EndImGuiPass() = 0;
		virtual void ShutdownImGui() = 0;

	protected:
		virtual void SetOnEventCallback(std::function<void(Event&)> callback) { EventCallback = callback; }
		virtual void OnEvent(Event& e) { m_Users->OnEvent(e); m_Devices->OnEvent(e); }

		virtual void Terminate(){}
		virtual void ToggleFullscreen() {}
		virtual void CursorVisible(bool) {}

		const GraphicsContext* GetGraphicsContextPtr() const { return p_GraphicsContext.get(); }

	protected:
		std::function<void(Event&)> EventCallback;

		std::unique_ptr<GraphicsContext> p_GraphicsContext;
		std::unique_ptr<TimeManager> m_Timer;
		std::unique_ptr<UserManager> m_Users;
		std::unique_ptr<DeviceManager> m_Devices;
		std::unique_ptr<AssetManager> m_AssetManager;
		std::shared_ptr<Log::Logger> m_Logger;
	};
}
