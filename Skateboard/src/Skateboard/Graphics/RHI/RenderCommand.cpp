#include "sktbdpch.h"
#include "RenderCommand.h"

#include "Skateboard/Graphics/RHI/RenderingApi.h"

namespace Skateboard
{
	RenderingApi* RenderCommand::Api{ nullptr };

	void RenderCommand::Init()
	{
		Api = GraphicsContext::Context->GetAPI();
	}
}
