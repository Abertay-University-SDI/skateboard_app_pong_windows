#pragma once
#include "sktbdpch.h"

#include "../Renderer3D/Shaders/Renderer3DCommonStructs.h"

#include "Skateboard/Mathematics.h"
#include "Skateboard/Graphics/Resources/Pipeline.h"
#include "Skateboard/Graphics/Resources/View.h"
#include "Skateboard/Scene/Components.h"

#include "Skateboard\Renderers\Renderer.h"

namespace Skateboard
{
	namespace Sprite
	{
		struct SpriteFrame
		{
			float2 uv_tl;
			float2 uv_br;
		};

		enum SpriteType : uint32_t
		{
			TEXTURED = 1,
		
			//DYNAMIC = 2,
			ALPHA_BLEND = 2,
		};

		ENUM_FLAG_OPERATORS(SpriteType);

		struct SpriteData
		{
			uint m_SpriteTextureID = {};

			float4 m_Colour = {0,0,0,0};

			float2	 m_Size		{1,1};
			float2	 m_Offset	{0,0};

			float2	 m_UVScale	{1,1};
			float2   m_UVOffset {0,0};
		};

		struct SpriteComponent 
		{
			SpriteData m_Data;
		};

		struct SpriteInstanceData
		{
			SpriteData		    m_SpriteData;
			matrix				m_Transform;
		};

		class Renderer2D final :
			public IRendererSystem
		{
			DISABLE_COPY_AND_MOVE(Renderer2D)
		public:
			Renderer2D() = default;
			~Renderer2D() = default;
		protected:

			Skateboard::ShaderInputLayoutRef RootSig;
			Skateboard::PipelineRef GraphicsPipeline;
			

			size_t InstanceDataBufferSize = 64*1024;
			size_t MaxInstanceCount = (InstanceDataBufferSize - ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT)) / sizeof(SpriteInstanceData);
			size_t InstanceCount = 0;
			

			Skateboard::MultiResource<BufferRef> InstanceDataBuffer;

			bool m_InstanceDataNeedsRebuilt;

			//inline view desc
			BufferViewDesc sbvdesc;
			BufferViewDesc cbvdesc;

			//resterizer
			SamplerDesc StaticSamplerDesc = SamplerDesc::InitAsDefaultTextureSampler();

			//counts offsets for dynamic data uploaded every frame
			size_t m_Offset = ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT);

			FrameData m_FrameData;
		public:
			void Init(uint32_t InstanceDataBufferSizeOverride)
			{
				InstanceDataBufferSize = (ROUND_UP(InstanceDataBufferSizeOverride, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT)) - ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT);
				MaxInstanceCount = InstanceDataBufferSize / sizeof(SpriteInstanceData);
				Init();
			}

			void SetFrameData(const FrameData& data)
			{
				m_FrameData = data;
			}

			FrameData GetFrameData()
			{
				return m_FrameData;
			}

		public:
			void Init() override;
			void Begin() override;
			void Update() override;
			void End() override;
			void Shutdown() override;

		protected:
			void OnRegistered(entt::registry* registry, const std::string name) override;
			void OnUnregistered(const std::string name) override;
		};

	}
}