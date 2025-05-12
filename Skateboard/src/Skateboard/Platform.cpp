#include "sktbdpch.h"
#include "Platform.h"

#if defined(SKTBD_PLATFORM_WINDOWS)
	#include "../../Platform/Windows/WindowsPlatform/WindowsPlatform.h"
#elif defined(SKTBD_PLATFORM_PLAYSTATION)
	#include "../../Platform/Playstation/PlaystationPlatform/PlaystationPlatform.h"
#endif

namespace Skateboard
{
	Platform& Platform::GetPlatform()
	{
#ifdef SKTBD_PLATFORM_WINDOWS
		static WindowsPlatform platform;
		return platform;
#elif defined(SKTBD_PLATFORM_PLAYSTATION)
		static PlaystationPlatform platform;
		return platform;
#else
#error Platform not yet supported.
#endif
	}

}