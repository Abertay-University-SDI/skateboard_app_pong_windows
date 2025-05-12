#pragma once
// The goal of this file is to have the engine handle the entry point for the client
// This is mainly because of the different platforms
// We can ignore the errors (if any)

namespace Skateboard
{
	class Application;
}

extern Skateboard::Application* Skateboard::CreateApplication(int argc, char** argv);

#ifdef SKTBD_PLATFORM_WINDOWS
#ifdef SKTBD_API_D3D12
//#include "../../Platform/Windows/WindowsPlatform/windowspch.h"
extern "C" { __declspec(dllexport) extern const uint32_t D3D12SDKVersion = 614; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = "D3D12\\"; }
#endif// SKTBD_API_D3D12

int main(int argc, char** argv)
{
	//Create Platform
	Skateboard::Platform::GetPlatform();

	std::unique_ptr<Skateboard::Application> app(Skateboard::CreateApplication(argc, argv));
	app->Run();
	return 0;
}
#elif defined(SKTBD_PLATFORM_PLAYSTATION)
#include <stdlib.h>

#define SKTBD_ENTRY_POINT

#include "Skateboard/Platform.h"

// We need to check cloning the project works first time for new clones, project configuration should work
int main(int argc, char** argv)
{
	Skateboard::Platform::GetPlatform();

	std::unique_ptr<Skateboard::Application> app(Skateboard::CreateApplication(argc, argv));
	app->Run();
	return 0;
}
#else
#error Skateboard only supports windows for now!
#endif