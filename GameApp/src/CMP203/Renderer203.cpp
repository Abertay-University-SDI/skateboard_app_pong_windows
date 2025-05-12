#include "Renderer203.h"
#include "Skateboard/Assets/AssetManager.h"

namespace CMP203
{
	void Renderer203::Init()
	{
		SKTBD_APP_INFO("Renderer 203 Init")

	//	m_DefaultTextureIDX = AssetManager::LoadTexture(L"assets/whitePixel.dds", "DefaultTexKey")->GetViewIndex();

		//Layout
		ShaderInputLayoutDesc Layout{};

		//instance index //0
		Layout.AddRootConstant(1, 0);

		//frame data //1
		Layout.AddConstantBufferView(1);

		//instance data //2
		Layout.AddShaderResourceView(0);

		//light data //3
		Layout.AddShaderResourceView(1);

		Layout.AddStaticSampler(StaticSamplerDesc,0);

		Layout.DescriptorsDirctlyAddresssed = true;
		Layout.SamplersDirectlyAddressed = true;
		Layout.CanUseInputAssembler = true;

		RootSig = ResourceFactory::CreateShaderInputLayout(Layout);

		RasterizationPipelineDesc Raster{};
		Raster.Rasterizer = RasterizerConfig::Default();
		Raster.Blend.AlphaToCoverage = false;
		Raster.Blend.IndependentBlendEnable = false;
		Raster.Blend.RTBlendConfigs[0].BlendEnable = false;
		Raster.Blend.RTBlendConfigs[0].RenderTargetWriteMask = 0xF;
		Raster.DepthStencil.DepthEnable = false;
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
		Raster.InputVertexLayout = Vertex::VertexLayout();
		Raster.SampleCount = 1;
		Raster.SampleQuality = 0;
		Raster.SampleMask = 1;

		Raster.SetVertexShader(L"CMP203_VertexShader");
		Raster.SetPixelShader(L"CMP203_PixelShader_Unlit");

		//piepline permutations
		for(uint32_t p = 0; p < PIPELINE_PERMUTATIONS; p++ )
		{
			auto RSPD = Raster;

			if (p & PipelineFlags::LIT)
			{
				RSPD.SetPixelShader(L"CMP203_PixelShader");
			}

			if (p & PipelineFlags::ALPHA_BLEND)
			{
				RSPD.Blend.AlphaToCoverage = true;
				RSPD.Blend.RTBlendConfigs[0].BlendEnable = true;
				RSPD.Blend.RTBlendConfigs[0].BlendOp = SKTBD_BlendOp_ADD;
				RSPD.Blend.RTBlendConfigs[0].SrcBlend = SKTBD_Blend_SRC_ALPHA;
				RSPD.Blend.RTBlendConfigs[0].DestBlend = SKTBD_Blend_INV_SRC_ALPHA;
				RSPD.Blend.RTBlendConfigs[0].BlendOpAlpha = SKTBD_BlendOp_ADD;
				RSPD.Blend.RTBlendConfigs[0].SrcBlendAlpha = SKTBD_Blend_SRC_ALPHA;
				RSPD.Blend.RTBlendConfigs[0].DestBlendAlpha = SKTBD_Blend_INV_SRC_ALPHA;
			}

			if (p & PipelineFlags::WIREFRAME)
			{
				RSPD.Rasterizer.Wireframe = true;
			}

			if (p & PipelineFlags::DEPTH_TEST)
			{
				RSPD.DepthStencil.DepthEnable = true;
			}

			PipelineDesc PiplDesc;
			PiplDesc.GlobalLayoutSignature = RootSig;
			PiplDesc.Type = PipelineType_Graphics;
			PiplDesc.TypeDesc = &RSPD;

			//L"Renderer203_Pipeline" + std::to_wstring(p), ,
			Pipelines[p] = ResourceFactory::CreatePipelineState(PiplDesc);
		}

		//NormalPipeline 

		Raster.DepthStencil.DepthEnable = true;
		Raster.SetGeometryShader(L"CMP203_NormalGS_GS");
		Raster.SetVertexShader(L"CMP203_NormalGS_VS");
		Raster.SetPixelShader(L"CMP203_NormalGS_PS");


		PipelineDesc PiplDesc;
		PiplDesc.GlobalLayoutSignature = RootSig;
		PiplDesc.Type = PipelineType_Graphics;
		PiplDesc.TypeDesc = &Raster;

		//L"Renderer203_N_Pipeline", ,
		NormalVisualizer = ResourceFactory::CreatePipelineState(PiplDesc);

		//create Buffers
		BufferDesc bufferdesc{};

		//Triangle Buffer
		bufferdesc.Init(DynamicBufferSize, ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead);
		TriangleDataBuffer.ForEach([&](BufferRef& ref) {ref = ResourceFactory::CreateBuffer(bufferdesc); });

		//InstanceData Buffer
		bufferdesc.Init(InstanceDataBufferSize, ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead);
		InstanceDataBuffer.ForEach([&](BufferRef& ref) {ref = ResourceFactory::CreateBuffer(bufferdesc); });
		
		//LightDataBuffer
		LightDataBuffer.ForEach([&](BufferRef& ref) {ref = ResourceFactory::CreateBuffer(bufferdesc); });


		//Create Views
		//Camera data
		cbvdesc.InitAsConstantBuffer<Frame>(0);

		//InstanceData
		sbvdesc.InitAsStructuredBuffer<InstanceData>(0, InstanceDataBufferSize / sizeof(InstanceData));

		//lightdata
		lightsbvdesc.InitAsStructuredBuffer<Light>(0, InstanceDataBufferSize / sizeof(Light));

		//compute default camera
		auto aspect = GraphicsContext::GetClientAspectRatio();
		m_CameraData.ProjectionMatrix = glm::perspectiveLH(glm::radians(90.f), aspect, 0.01f, 100.f);
		m_CameraData.ViewMatrix = glm::lookAtLH(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		
		m_DefaultInstanceData.TextureIndex = m_DefaultTextureIDX;

		//default Instance
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer[0].get(), 0, sizeof(InstanceData), &m_DefaultInstanceData);
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer[1].get(), 0, sizeof(InstanceData), &m_DefaultInstanceData);
		GraphicsContext::CopyDataToBuffer(InstanceDataBuffer[2].get(), 0, sizeof(InstanceData), &m_DefaultInstanceData);
	}

	void Renderer203::Begin()
	{
		//Move Onto Next frame In Buffer
		TriangleDataBuffer.IncrementCounter();

		InstanceDataBuffer.IncrementCounter();

		LightDataBuffer.IncrementCounter();

		//reset the buffer Offset
		m_Offset = ROUND_UP(sizeof(Frame), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT);

		m_InstanceDataForks = 1;

		RenderCommand::SetInputLayoutGraphics(RootSig.get());
		RenderCommand::SetPipelineState(Pipelines[m_PipelineSelection].get());
	}

	void Renderer203::End()
	{
		m_Frame.Matrices = m_CameraData;
		m_Frame.LightCount = m_Lights.size();
		m_Frame.CameraMatrix = glm::inverse(m_CameraData.ViewMatrix);

		//update the frame data buffer section
		GraphicsContext::CopyDataToBuffer(TriangleDataBuffer.Get().get(), 0, sizeof(Frame), &m_Frame);

		//send lightdata to gpu
		GraphicsContext::CopyDataToBuffer(LightDataBuffer.Get().get(), 0, sizeof(Light) * m_Frame.LightCount, m_Lights.data());
	}

	void Renderer203::Shutdown()
	{

	}

	void Renderer203::SetFrameData(FrameData newData)
	{
		m_CameraData = newData;
	}

	void Renderer203::DrawVBIB(VertexBufferView* vb, IndexBufferView* ib, InstanceData* data)
	{
		if (m_Pipeline_dirty)
		{
			m_Pipeline_dirty = false;
			RenderCommand::SetPipelineState(Pipelines[m_PipelineSelection].get());
		}

		if (data)
		{
			GraphicsContext::CopyDataToBuffer(InstanceDataBuffer.Get().get(), sizeof(InstanceData) * m_InstanceDataForks, sizeof(InstanceData), data);
			RenderCommand::SetInline32bitDataGraphics(0, &m_InstanceDataForks, 1);
			++m_InstanceDataForks;
		}
		else
		{
			//default value
			int d = 0;
			RenderCommand::SetInline32bitDataGraphics(0, &d, 1);
		}

		RenderCommand::SetInlineResourceViewGraphics(1, TriangleDataBuffer.Get().get(), cbvdesc, ViewAccessType_ConstantBuffer);
		RenderCommand::SetInlineResourceViewGraphics(2, InstanceDataBuffer.Get().get(), sbvdesc, ViewAccessType_GpuRead);
		RenderCommand::SetInlineResourceViewGraphics(3, LightDataBuffer.Get().get(), lightsbvdesc, ViewAccessType_GpuRead);

		RenderCommand::SetIndexBuffer(ib);
		RenderCommand::SetVertexBuffer(vb, 1);

		RenderCommand::SetPrimitiveTopology(Topology);

		RenderCommand::DrawIndexed(0, 0, ib->m_IndexCount);

		if (m_VisualiseNormals)
		{
			RenderCommand::SetPipelineState(NormalVisualizer.get());
			RenderCommand::DrawIndexed(0, 0, ib->m_IndexCount);
			RenderCommand::SetPipelineState(Pipelines[m_PipelineSelection].get());
		}
	}

	void Renderer203::DrawVertices(Vertex* vertexBuffer, size_t vertexcount, uint32_t* indexbuffer, size_t indexcount, InstanceData* data)
	{
		size_t VBSize = vertexcount * Vertex::VertexLayout().GetStride();
		size_t IBSize = sizeof(int32_t) * indexcount;
		
		GraphicsContext::CopyDataToBuffer(TriangleDataBuffer.Get().get(),m_Offset,VBSize, vertexBuffer);
		GraphicsContext::CopyDataToBuffer(TriangleDataBuffer.Get().get(),m_Offset + VBSize, IBSize, indexbuffer);

		Skateboard::VertexBufferView vbv{};
		Skateboard::IndexBufferView ibv{};

		vbv.m_Offset = m_Offset;
		vbv.m_ParentResource = TriangleDataBuffer.Get();
		vbv.m_VertexCount = vertexcount;
		vbv.m_VertexStride = Vertex::VertexLayout().GetStride();

		ibv.m_Offset = m_Offset + VBSize;
		ibv.m_Format = IndexFormat::bit32;
		ibv.m_ParentResource = TriangleDataBuffer.Get();
		ibv.m_IndexCount = indexcount;
		
		m_Offset += VBSize + IBSize;

		DrawVBIB(&vbv, &ibv, data);
	}


	void Renderer203::DrawSphere(float radius, int n_stacks, int n_slices, float3 v_colour, CMP203::InstanceData* transformData)
	{
		//vertex buffer
		std::vector<Vertex> vertices;

		//index buffer
		std::vector<uint32_t> indices;

		// add top vertex
		Vertex v0;
		v0.Position = { 0.f, radius, 0.f };
		//v0.Colour = { 0.5f, 0.f, 0.f };
		v0.Colour = v_colour;
		v0.UV = { 0.5, 0 };
		v0.Normal = { 0, 1, 0 };
		vertices.push_back(v0);

		// generate vertices per stack / slice
		for (int i = 0; i < n_stacks - 1; i++)
		{
			float phi = SKTBD_PI * float(i + 1) / float(n_stacks);
			for (int j = 0; j < n_slices; j++)
			{
				float theta = 2.0 * SKTBD_PI * float(j) / float(n_slices);
				float x = radius * std::sin(phi) * std::cos(theta);
				float y = radius * std::cos(phi);
				float z = radius * std::sin(phi) * std::sin(theta);
				Vertex v;
				v.Position = { x, y, z };
				float lightness = glm::fclamp((x / radius) + 0.2f, 0.f, 1.f);

				//v.Colour = { lightness * (float(i) / (float)n_stacks), 0, lightness * (1 - float(i) / (float)n_stacks) };
				v.Colour = v_colour;
				v.UV = { (float)j * (1 / (float)n_stacks), (float)i * (1.f / (float)n_slices) };
				v.Normal = glm::normalize(float3(x, y, z));
				vertices.push_back(v);

			}
		}

		// add bottom vertex
		Vertex v1;
		v1.Position = { 0, -radius, 0 };
		//v1.Colour = { 0.5f, 0.f, 0.f };
		v1.Colour = v_colour;
		v0.UV = { 0.5, 1 };
		v1.Normal = { 0, -1, 0 };
		vertices.push_back(v1);


		// add top / bottom triangles
		for (int i = 0; i < n_slices; ++i)
		{
			uint32_t i0 = i + 1;
			uint32_t i1 = (i + 1) % n_slices + 1;

			indices.push_back(0);
			indices.push_back(i0);
			indices.push_back(i1);

			i0 = i + n_slices * (n_stacks - 2) + 1;
			i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;

			indices.push_back(vertices.size() - 1);
			indices.push_back(i1);
			indices.push_back(i0);
		}

		// add quads per stack / slice
		for (int j = 0; j < n_stacks - 2; j++)
		{
			uint32_t j0 = j * n_slices + 1;
			uint32_t j1 = (j + 1) * n_slices + 1;
			for (int i = 0; i < n_slices; i++)
			{
				uint32_t i0 = j0 + i;
				uint32_t i1 = j0 + (i + 1) % n_slices;
				uint32_t i2 = j1 + (i + 1) % n_slices;
				uint32_t i3 = j1 + i;

				indices.push_back(i3);
				indices.push_back(i2);
				indices.push_back(i0);

				indices.push_back(i2);
				indices.push_back(i1);
				indices.push_back(i0);
			}
		}

		DrawVertices(vertices.data(), vertices.size(), indices.data(), indices.size(), transformData);
	}
}