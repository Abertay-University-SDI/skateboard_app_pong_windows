#pragma once
#include "sktbdpch.h"
#include "Skateboard/Mathematics.h"
#include "Skateboard/Graphics/Resources/Pipeline.h"
#include "Skateboard/Graphics/Resources/View.h"
#include "Skateboard/Scene/Components.h"

#include "Shaders/Renderer3DCommonStructs.h"
#include "Skateboard/Renderers/Renderer.h"
#include "Skateboard/Assets/AssetManager.h"

#include "Shaders/Renderer3DCommonStructs.h"

namespace Skateboard
{
	enum MaterialType : uint8_t
	{
		UNLIT,
		LIT,
		//	TRANSPARENT,
		//	
		MATERIAL_COUNT
	};

	struct Material
	{
		Material() : m_type(UNLIT), m_visible(true), m_AlbedoTexture(AssetManager::GetDefaultTexture(TextureDimension_Texture2D)), m_NormalTexture(AssetManager::GetDefaultTexture(TextureDimension_Texture2D)), m_shininessPow(256)
		{

		};

		MaterialType m_type;
		bool m_visible;

		//textures
		Texture m_AlbedoTexture;
		Texture m_NormalTexture;

		//params
		float m_shininessPow;
	};

	struct StaticMeshComponent
	{
		friend class Renderer3D;

		StaticMeshComponent() = default;
		StaticMeshComponent(Mesh* mesh) : m_mesh(mesh) {
			if (mesh) { m_MeshMaterials.resize(mesh->GetPrimitiveCount()); m_LocalPrimitiveTransform.resize(mesh->GetPrimitiveCount(), Transform()); }
		}

		void SetMesh(Mesh* mesh) { m_mesh = mesh; if (mesh) { m_MeshMaterials.resize(mesh->GetPrimitiveCount()); m_LocalPrimitiveTransform.resize(mesh->GetPrimitiveCount()); } }

		Mesh* GetMesh() const { return m_mesh; }

		uint32_t GetPrimitiveCount() const { if (m_mesh) return m_mesh->GetPrimitiveCount(); else return 0; }

		void SetMaterial(uint32_t Primitive_idx, const Material& new_material)
		{
			//if this fails mesh doesnt have that many primitives
			ASSERT(Primitive_idx < m_mesh->GetPrimitiveCount());
			m_MeshMaterials[Primitive_idx] = new_material;
		}

		void SetLocalTransform(uint32_t Primitive_idx, const Transform& new_LocalTransform)
		{
			//if this fails mesh doesnt have that many primitives
			ASSERT(Primitive_idx < m_mesh->GetPrimitiveCount());
			m_LocalPrimitiveTransform[Primitive_idx] = new_LocalTransform;
		}

		Transform& GetLocalTransform(uint32_t Primitive_idx) {
			ASSERT(Primitive_idx < m_mesh->GetPrimitiveCount());
			return m_LocalPrimitiveTransform[Primitive_idx];
		}

		Material& GetMaterial(uint32_t Primitive_idx) {
			ASSERT(Primitive_idx < m_mesh->GetPrimitiveCount());
			return m_MeshMaterials[Primitive_idx];
		}

	protected:
		std::vector<Material> m_MeshMaterials;
		std::vector<Transform> m_LocalPrimitiveTransform;
		Mesh* m_mesh;
	};

	struct LightComponent
	{
		LightComponent()
		{
			m_Light.LightType = LightDirectional;
			m_Light.LightDirection = float3(1, -1, 1);
			m_Light.DiffuseColour  = float4(0.5, 0.5, 0.5,1);
		}

		LightComponent(LightType type)
		{
			switch (type)
			{
			case LightDirectional:
				m_Light.LightType = LightDirectional;
				m_Light.LightDirection = float3(1, -1, 1);
				m_Light.DiffuseColour = float4(0.5, 0.5, 0.5, 1);
				break;
			case LightPoint:
				m_Light.LightType = LightPoint;
				m_Light.DiffuseColour = float4(0.5, 0.5, 0.5, 1);
				m_Light.LightPosition = float3(0,0,0);
				m_Light.ConstantAttenuation = 0.05f;
				m_Light.LinearAttenuation = 0.01f;
				m_Light.SquareAttenuation = 0.001f;
				m_Light.CutOffDistance = 100.f;
				break;
			case LightSpot:
				m_Light.LightType = LightSpot;
				m_Light.LightDirection = float3(0, 1, 0);
				m_Light.DiffuseColour = float4(0.5, 0.5, 0.5, 1);
				m_Light.LightPosition = float3(0, 0, 0);
				m_Light.ConstantAttenuation = 0.05f;
				m_Light.LinearAttenuation = 0.01f;
				m_Light.SquareAttenuation = 0.001f;
				m_Light.CutOffDistance = 100.f;
				m_Light.InnerCone = 30;
				m_Light.OuterCone = 90;
				m_Light.FalloffPower = 2;
				break;
			}
			
		}

		Light m_Light;
		bool m_active = true;
	};

	typedef VS_IN Vertex;

	class Renderer3D final :
		public IRendererSystem
	{
		DISABLE_COPY_AND_MOVE(Renderer3D)
	public:
		Renderer3D() = default;
		~Renderer3D() = default;
		//Vertex Attributes
	public:
		//These match for now 
		static BufferLayout VertexLayout()
		{
			return {
					{ POSITION,	 ShaderDataType_::Float3, },
					{ NORMAL,     ShaderDataType_::Float3  },
					{ TANGENT,     ShaderDataType_::Float3  },
					{ TEXCOORD,   ShaderDataType_::Float2  }
			};
		}

		static BufferLayout ImportLayout()
		{
			return {
					{ POSITION,	ShaderDataType_::Float3 },
					{ NORMAL,     ShaderDataType_::Float3 },
					{ TANGENT,     ShaderDataType_::Float3 },
					{ TEXCOORD,	ShaderDataType_::Float2 },
			};
		}
	private:
		Skateboard::ShaderInputLayoutRef RootSig;

		size_t InstanceDataBufferSize = ROUND_UP(64 * 1024, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT);

		//lights will be suballocated from Instance Data
		size_t MaxLightCount = 16;
		size_t MaxInstanceCount = (InstanceDataBufferSize - ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT) - ROUND_UP(MaxLightCount * sizeof(Light), GraphicsConstants::BUFFER_ALIGNMENT)) / sizeof(InstanceData);

		size_t LightCount = 0;
		size_t InstanceCount = 0;

		Skateboard::MultiResource<BufferRef> InstanceDataBuffer;

		std::array<uint32_t, MaterialType::MATERIAL_COUNT> MaterialInstanceCounts;
		std::array<PipelineRef, MaterialType::MATERIAL_COUNT> MaterialPipelines;

		//inline view desc
		BufferViewDesc InstanceDataView;
		BufferViewDesc FrameDataView;

		//Descriptors
		MultiResource<BufferSRVRef> LightDataViews;

		//resterizer
		SamplerDesc StaticSamplerDesc = SamplerDesc::InitAsDefaultTextureSampler();

		//Frame Data copy
		FrameData m_FrameData;
	public:

		void Init(uint32_t InstanceDataBufferSizeOverride, uint32 MaxLightCountOverride = 16)
		{
			InstanceDataBufferSize = ROUND_UP(InstanceDataBufferSizeOverride, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT);
			MaxLightCount = MaxLightCountOverride;
			MaxInstanceCount = (InstanceDataBufferSize - ROUND_UP(sizeof(FrameData), GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT) - ROUND_UP(MaxLightCount * sizeof(Light), GraphicsConstants::BUFFER_ALIGNMENT)) / sizeof(InstanceData);
			Init();
		}

		void SetAmbientLight(const float4& AmbientLight)
		{
			m_FrameData.AmbientLight = AmbientLight;
		}

		void SetFrameData(const FrameData& data)
		{
			m_FrameData = data;
		}

		FrameData GetFrameData()
		{
			return m_FrameData;
		}

		void OnComponentAdded();
		void OnComponentTransformUpdated();
		void OnComponentRemoved();

	public:
		void Init() override;
		void Begin() override;
		void Update() override;
		void End() override;
		void Shutdown() override;

	protected:
		/*void OnMeshComponentAdded(StaticMeshComponent& component);
		void OnMeshComponentUpdated(StaticMeshComponent& component);
		void OnMeshComponentRemoved(StaticMeshComponent& component);*/

		void LightAdded(entt::registry&, entt::entity);
		void LightRemoved(entt::registry&, entt::entity);
		void LightUpdated(entt::registry&, entt::entity);


		void OnRegistered(entt::registry* registry, const std::string name) override;
		void OnUnregistered(const std::string name) override;
	};
}