#include "Renderer2D.h"

#define SKTBD_LOG_COMPONENT "SpriteRenderer2D"
#include "Skateboard/Log.h"
#include "Skateboard/Assets/AssetManager.h"
#include "Skateboard/Graphics/RHI/RenderCommand.h"

#include "Skateboard/Graphics/RHI/ResourceFactory.h"

namespace Skateboard::Sprite {

	void Renderer2D::Init()
	{
		SKTBD_APP_INFO("Renderer 2D Init")

		ShaderInputLayoutDesc Layout{};

		//frame data //0
		Layout.AddConstantBufferView(0);

		//instance data //1
		Layout.AddShaderResourceView(0);

		Layout.AddStaticSampler(SamplerDesc::InitAsDefaultTextureSampler(), 0);

		Layout.DescriptorsDirctlyAddresssed = true;
		Layout.SamplersDirectlyAddressed = false;
		Layout.CanUseInputAssembler = true;

		RootSig = ResourceFactory::CreateShaderInputLayout(Layout);

		RasterizationPipelineDesc Raster{};
		Raster.Rasterizer = RasterizerConfig::Default();
	//	Raster.Rasterizer.Cull = Cull_NONE;
		Raster.Blend.AlphaToCoverage = false;
		Raster.Blend.IndependentBlendEnable = false;
		Raster.Blend.RTBlendConfigs[0].BlendEnable = false;
		Raster.Blend.RTBlendConfigs[0].RenderTargetWriteMask = 0xF;
		Raster.DepthStencil.DepthEnable = true;
		Raster.DepthStencil.BackFace.StencilDepthFailOp = SKTBD_StencilOp_KEEP;
		Raster.DepthStencil.BackFace.StencilFailOp = SKTBD_StencilOp_KEEP;
		Raster.DepthStencil.BackFace.StencilFunc = SKTBD_CompareOp_ALWAYS;
		Raster.DepthStencil.BackFace.StencilPassOp = SKTBD_StencilOp_KEEP;
		Raster.DepthStencil.DepthFunc = SKTBD_CompareOp_LESS;
		Raster.DepthStencil.DepthWriteAll = true;
		Raster.DepthStencil.FrontFace = Raster.DepthStencil.BackFace;
		Raster.DepthStencil.StencilEnable = false;
		Raster.DepthstencilTargetFormat = DataFormat_DEFAULT_DEPTHSTENCIL;
		Raster.InputPrimitiveType = PrimitiveTopologyType_Triangle;
		Raster.RenderTargetCount = 1;
		Raster.RenderTargetDataFormats[0] = DataFormat_DEFAULT_BACKBUFFER;
		Raster.InputVertexLayout = BufferLayout();
		Raster.SampleCount = 1;
		Raster.SampleQuality = 0;
		Raster.SampleMask = 1;

		//Blending 
		//Raster.Blend.AlphaToCoverage = true;
		Raster.Blend.RTBlendConfigs[0].BlendEnable = true;
		Raster.Blend.RTBlendConfigs[0].BlendOp = SKTBD_BlendOp_ADD;
		Raster.Blend.RTBlendConfigs[0].SrcBlend = SKTBD_Blend_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].DestBlend = SKTBD_Blend_INV_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].BlendOpAlpha = SKTBD_BlendOp_ADD;
		Raster.Blend.RTBlendConfigs[0].SrcBlendAlpha = SKTBD_Blend_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].DestBlendAlpha = SKTBD_Blend_INV_SRC_ALPHA;

		Raster.SetVertexShader(L"CMP208/SpriteVertexShader");
		Raster.SetPixelShader(L"CMP208/SpritePixelShader");

		PipelineDesc PiplDesc;
		PiplDesc.GlobalLayoutSignature = RootSig;
		PiplDesc.Type = PipelineType_Graphics;
		PiplDesc.TypeDesc = &Raster;

		GraphicsPipeline = ResourceFactory::CreatePipelineState(PiplDesc);

		//Buffers

		//create Buffers
		BufferDesc bufferdesc{};

		//InstanceData Buffer
		bufferdesc.Init(InstanceDataBufferSize, ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead);
		InstanceDataBuffer.ForEach([&](BufferRef& ref) {ref = ResourceFactory::CreateBuffer(bufferdesc); });

		//Create Views
		//Camera data
		cbvdesc.InitAsConstantBuffer<FrameData>(0);

		//InstanceData
		sbvdesc.InitAsStructuredBuffer<SpriteInstanceData>(m_Offset, InstanceDataBufferSize / sizeof(SpriteInstanceData));

		auto aspect = GraphicsContext::GetClientAspectRatio();
		m_FrameData.Projection = glm::perspectiveLH(glm::radians(90.f), aspect, 0.01f, 100.f);
		m_FrameData.View = glm::lookAtLH(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}

	void Renderer2D::Begin()
	{
		//Set Descriptors/Pipeline Vars
		RenderCommand::SetInputLayoutGraphics(RootSig.get());
		RenderCommand::SetInlineResourceViewGraphics(0, InstanceDataBuffer.Get().get(), cbvdesc, ViewAccessType_ConstantBuffer);
		RenderCommand::SetInlineResourceViewGraphics(1, InstanceDataBuffer.Get().get(), sbvdesc, ViewAccessType_GpuRead);

		RenderCommand::SetPipelineState(GraphicsPipeline.get());
		RenderCommand::SetPrimitiveTopology(SKTBD_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Renderer2D::Update()
	{
		//next frame
		++InstanceDataBuffer;
		InstanceCount = 0;

		for (auto [name, reg] : m_Registries)
		{
			auto group = reg->group<>(entt::get<TransformComponent, CameraComponent>);
			group.each([&](TransformComponent Trans, CameraComponent& Cam)
				{
					if (Cam.Primary && Cam.Camera) {
						m_FrameData.View = Cam.Camera->GetViewMatrix();
						m_FrameData.Projection = Cam.Camera->GetProjectionMatrix();
					}
				});
		}

		//Copy Camera
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), 0, sizeof(FrameData), &m_FrameData);
		

		//Copy Instance data
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), m_Offset, InstanceDataBufferSize - m_Offset,
		[&](void* dest)
		{
				for(auto [name, reg] : m_Registries)
				{
					auto group = reg->group<>(entt::get<TransformComponent, SpriteComponent>);
					group.each([&](TransformComponent& Transform, SpriteComponent& sprite)
					{
							if (InstanceCount > MaxInstanceCount) return;

							static_cast<SpriteInstanceData*>(dest)[InstanceCount].m_Transform = Transform.GetTransform();
							static_cast<SpriteInstanceData*>(dest)[InstanceCount].m_SpriteData = sprite.m_Data;
							InstanceCount++;
					});
				}
		});
	}

	void Renderer2D::End()
	{
		if(InstanceCount)
		RenderCommand::DrawInstanced(0, 6, InstanceCount, 0);
	}

	void Renderer2D::Shutdown()
	{

	}

	void Renderer2D::OnRegistered(entt::registry* registry, const std::string name)
	{
		//registry->on_construct<SpriteComponent>();
	}

	void Renderer2D::OnUnregistered(const std::string name)
	{
		//m_Registries[name]->on_construct<SpriteComponent>();
	}
}
