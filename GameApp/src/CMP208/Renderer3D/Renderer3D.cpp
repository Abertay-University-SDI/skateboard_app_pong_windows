
#include "Renderer3D.h"

#define SKTBD_LOG_COMPONENT "Renderer3D"
#include "Skateboard/Log.h"

#include "Skateboard/Assets/AssetManager.h"
#include "Skateboard/Graphics/RHI/RenderCommand.h"
#include "Skateboard/Graphics/RHI/ResourceFactory.h"

namespace CMP203
{
	enum LightType : uint32_t;
}

namespace Skateboard {

	void Renderer3D::OnComponentAdded()
	{
	}

	void Renderer3D::OnComponentTransformUpdated()
	{
	}

	void Renderer3D::OnComponentRemoved()
	{
	}

	void Renderer3D::Init()
	{
		SKTBD_APP_INFO("Renderer 2D Init")

		ShaderInputLayoutDesc Layout{};

		//frame data //0
		Layout.AddConstantBufferView(0);

		//instance data //1
		Layout.AddShaderResourceView(0);

#ifdef SKTBD_PLATFORM_WINDOWS //workaround for Instance offset, build it into the syntax and dont implement it Microsoft????
		Layout.AddRootConstant(1,1);
#endif


		Layout.AddStaticSampler(SamplerDesc::InitAsDefaultTextureSampler(), 0);

		Layout.DescriptorsDirctlyAddresssed = true;
		Layout.SamplersDirectlyAddressed = false;
		Layout.CanUseInputAssembler = true;

		RootSig = ResourceFactory::CreateShaderInputLayout(Layout);

		RasterizationPipelineDesc Raster{};
		Raster.Rasterizer = RasterizerConfig::Default();
		Raster.Rasterizer.Cull = Cull_BACK;
		Raster.Rasterizer.FrontCC = false;
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
		Raster.InputVertexLayout = VertexLayout();
		Raster.SampleCount = 1;
		Raster.SampleQuality = 0;
		Raster.SampleMask = 1;

		//Blending 
		Raster.Blend.AlphaToCoverage = false;
		Raster.Blend.RTBlendConfigs[0].BlendEnable = false;
		Raster.Blend.RTBlendConfigs[0].BlendOp = SKTBD_BlendOp_ADD;
		Raster.Blend.RTBlendConfigs[0].SrcBlend = SKTBD_Blend_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].DestBlend = SKTBD_Blend_INV_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].BlendOpAlpha = SKTBD_BlendOp_ADD;
		Raster.Blend.RTBlendConfigs[0].SrcBlendAlpha = SKTBD_Blend_SRC_ALPHA;
		Raster.Blend.RTBlendConfigs[0].DestBlendAlpha = SKTBD_Blend_INV_SRC_ALPHA;

		Raster.SetVertexShader(L"CMP208/MeshVertexShader");
		Raster.SetPixelShader(L"CMP208/MeshPixelShader_Unlit");

		PipelineDesc PiplDesc;
		PiplDesc.GlobalLayoutSignature = RootSig;
		PiplDesc.Type = PipelineType_Graphics;
		PiplDesc.TypeDesc = &Raster;

		MaterialPipelines[UNLIT] = ResourceFactory::CreatePipelineState(PiplDesc);

		Raster.SetPixelShader(L"CMP208/MeshPixelShader"); 
		MaterialPipelines[LIT] = ResourceFactory::CreatePipelineState(PiplDesc);

		//Buffers 

		//create Buffers
		BufferDesc bufferdesc{};

		//InstanceData Buffer
		bufferdesc.Init(InstanceDataBufferSize, ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead);
		InstanceDataBuffer.ForEach([&](BufferRef& ref) {ref = ResourceFactory::CreateBuffer(bufferdesc); });

		//Create Views

		size_t m_Offset = 0;

		//Camera data
		FrameDataView.InitAsConstantBuffer<FrameData>(m_Offset);

		m_Offset += ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT);

		if (MaxLightCount)
		{
			BufferViewDesc Lightview;
			Lightview.InitAsStructuredBuffer<Light>(4, MaxLightCount);
			LightDataViews.ForEach([&](BufferSRVRef& view)->void { view = ResourceFactory::CreateBufferShaderResourceView(Lightview, InstanceDataBuffer.Get()); ++InstanceDataBuffer; });

			m_Offset = ROUND_UP(sizeof(Light) * (MaxLightCount+4), GraphicsConstants::BUFFER_ALIGNMENT);
		}

		//InstanceData
		InstanceDataView.InitAsStructuredBuffer<InstanceData>(m_Offset, InstanceDataBufferSize / sizeof(InstanceData));

		auto aspect = GraphicsContext::GetClientAspectRatio();
		m_FrameData.Projection = glm::perspectiveLH(glm::radians(90.f), aspect, 0.01f, 100.f);
		m_FrameData.View = glm::lookAtLH(glm::vec3(0, 5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		m_FrameData.Camera = glm::inverse(m_FrameData.View);
		m_FrameData.AmbientLight = float4(0.1, 0.1, 0.1,1.f);
	}

	void Renderer3D::Begin()
	{
		//Set Descriptors/Pipeline Vars
		RenderCommand::SetInputLayoutGraphics(RootSig.get());
		RenderCommand::SetInlineResourceViewGraphics(0, InstanceDataBuffer.Get().get(), FrameDataView, ViewAccessType_ConstantBuffer);
		RenderCommand::SetInlineResourceViewGraphics(1, InstanceDataBuffer.Get().get(), InstanceDataView, ViewAccessType_GpuRead);

		RenderCommand::SetPrimitiveTopology(SKTBD_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Renderer3D::Update()
	{
		//next frame
		++InstanceDataBuffer;
		++LightDataViews;

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

		size_t m_Offset = 0;

		m_Offset = ROUND_UP(sizeof(Light) * (4), GraphicsConstants::BUFFER_ALIGNMENT);

		auto lightidx = 0u;
		//Copy Light Data
		if (LightCount && MaxLightCount)
		{
			GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), m_Offset, InstanceDataBufferSize - m_Offset,
				[&](void* dest)
				{
					for (auto [name, reg] : m_Registries)
					{
						auto view = reg->view<LightComponent>();

						for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
							auto& lightComp = view.get<LightComponent>(*it);
							auto& transform = reg->get<TransformComponent>(*it);

							if(lightComp.m_active)
							{
								auto& light = static_cast<Light*>(dest)[lightidx];

								//based on light type we need to compute the position and direction based on the transform
								vec3 cam_scale;
								vec3 cam_translation;
								quat cam_rotation;
								vec3 cam_skew;
								vec4 cam_persp;

								glm::decompose(m_FrameData.Camera, cam_scale, cam_rotation, cam_translation, cam_skew, cam_persp);

								light = lightComp.m_Light;

								switch (lightComp.m_Light.LightType)
								{
								case LightDirectional:
									light.LightPosition = cam_translation;
									light.LightDirection = rotate(transform.m_transform.Rotation, lightComp.m_Light.LightDirection);
									break;
								case LightPoint:
									light.LightPosition = float4(lightComp.m_Light.LightPosition,0) * transform.m_transform.AsMatrix();
									light.LightDirection = rotate(transform.m_transform.Rotation, lightComp.m_Light.LightDirection);
									break;
								case LightSpot:
									light.LightPosition = float4(lightComp.m_Light.LightPosition, 0) * transform.m_transform.AsMatrix();
									light.LightDirection = rotate(transform.m_transform.Rotation, lightComp.m_Light.LightDirection);
									break;
								default: ;
								}

								//guard clause
								if (lightidx >= MaxLightCount) return;

								++lightidx;
							}
						}
					}
				});
		}

		m_FrameData.LightCount = lightidx;
		m_FrameData.LightsBuffer = LightDataViews.Get()->GetViewIndex();

		//Copy Camera // needs to be done after the light because frame data contains light buffer information
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), 0, sizeof(FrameData), &m_FrameData);


		m_Offset = ROUND_UP(sizeof(Light) * (MaxLightCount+4), GraphicsConstants::BUFFER_ALIGNMENT);

		//Copy Instance data and immediately create a call to render said instance
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), m_Offset, InstanceDataBufferSize - m_Offset,
			[&](void* dest)
			{
				for (auto [name, reg] : m_Registries)
				{
					auto group = reg->view<StaticMeshComponent>();

					for (auto it = group.rbegin(), last = group.rend(); it != last; ++it) {
						auto& mesh = group.get<StaticMeshComponent>(*it);
						auto& TransformCmp = reg->get<TransformComponent>(*it);

						if (InstanceCount > MaxInstanceCount) return;

						int idx = 0;
						for (auto& i : mesh.m_mesh->GetPrimitives())
						{
							if (mesh.m_MeshMaterials[idx].m_visible)
							{
								auto& instanceData = static_cast<InstanceData*>(dest)[InstanceCount];
								instanceData.m_transform = TransformCmp.GetTransform() * mesh.m_LocalPrimitiveTransform[idx].AsMatrix();
								instanceData.AlbedoTextureID = mesh.m_MeshMaterials[idx].m_AlbedoTexture->GetViewIndex();
								instanceData.NormalTextureID = mesh.m_MeshMaterials[idx].m_NormalTexture->GetViewIndex();
								instanceData.shininess = mesh.m_MeshMaterials[idx].m_shininessPow;

								RenderCommand::SetIndexBuffer(&i.IndexBuffer);
								RenderCommand::SetVertexBuffer(&i.VertexBuffer, 1, 0);

								RenderCommand::SetPipelineState(MaterialPipelines[mesh.m_MeshMaterials[idx].m_type].get());

#ifdef SKTBD_PLATFORM_WINDOWS	//workaround for StartingInstance offset, build it into the syntax and dont implement it Microsoft???? WTF
								RenderCommand::SetInline32bitDataGraphics(2, &InstanceCount, 1);
#endif

								RenderCommand::DrawIndexedInstanced(0, 0, InstanceCount, i.IndexBuffer.m_IndexCount, 1);

								InstanceCount++;
							}
							++idx;
						}
					}
				}

				/*SKTBD_LOG_INFO("RENDERER3D", "Instance count {}, MaxInstanceCount {}", InstanceCount, MaxInstanceCount);*/
			});
	}

	void Renderer3D::End()
	{
	}

	void Renderer3D::Shutdown()
	{

	}

	void Renderer3D::LightAdded(entt::registry& reg, entt::entity ent)
	{
		auto& cmp = reg.get<LightComponent>(ent);
		if (cmp.m_active) LightCount++;
	}

	void Renderer3D::LightRemoved(entt::registry& reg, entt::entity ent)
	{
		auto& cmp = reg.get<LightComponent>(ent);
		if (cmp.m_active) LightCount++;
	}

	void Renderer3D::LightUpdated(entt::registry& reg, entt::entity ent)
	{
		auto& cmp = reg.get<LightComponent>(ent);
		if (cmp.m_active) LightCount++;

		//matrix View = glm::lookAtLH(cmp.Light.LightPosition, cmp.Light.LightPosition+light)
	}

	void Renderer3D::OnRegistered(entt::registry* registry, const std::string name)
	{
		if (MaxLightCount)
		{
			registry->on_construct<LightComponent>().connect<&Renderer3D::LightAdded>(this);
			registry->on_update<LightComponent>().connect<&Renderer3D::LightUpdated>(this);
			registry->on_destroy<LightComponent>().connect<&Renderer3D::LightRemoved>(this);

			auto lights = registry->view<LightComponent>();
			lights.each([&](const LightComponent& light) {if (light.m_active)LightCount++; });
		}
	}

	void Renderer3D::OnUnregistered(const std::string name)
	{
		if (MaxLightCount)
		{
			auto& registry = m_Registries[name];

			registry->on_construct<LightComponent>().disconnect<&Renderer3D::LightAdded>(this);
			registry->on_update<LightComponent>().disconnect<&Renderer3D::LightUpdated>(this);
			registry->on_destroy<LightComponent>().disconnect<&Renderer3D::LightRemoved>(this);

			auto lights = registry->view<LightComponent>();
			lights.each([&](const LightComponent& light) {if (light.m_active)LightCount--; });
		}
	}
}
