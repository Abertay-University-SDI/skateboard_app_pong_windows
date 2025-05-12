#include "Raytracer.h"

#include "CMP203/Renderer203.h"
#include "Skateboard/Assets/AssetManager.h"
#include "Skateboard/Graphics/RHI/RenderCommand.h"

void Raytracer::Init()
{
	//ROOT SIG

	//Layout
	ShaderInputLayoutDesc Layout{};

	//frame data
	Layout.AddConstantBufferView(1);
	
	//Layout.AddStaticSampler(SamplerDesc::InitAsDefaultTextureSampler(), 0);

	Layout.DescriptorsDirctlyAddresssed = true;
	Layout.SamplersDirectlyAddressed = true;
	Layout.CanUseInputAssembler = false; // dont need that for raytracing

	RootSignature = ResourceFactory::CreateShaderInputLayout(Layout);

	//ACCELERATION STRUCTURES

	BufferLayout ImportLayout{ {POSITION, ShaderDataType_::Float3}, {NORMAL,ShaderDataType_::Float3}, {TEXCOORD, ShaderDataType_::Float2} };
	auto torus = AssetManager::LoadModel(L"assets/models/torus", "torus", ImportLayout);

	GeometryDesc torusDesc;
	torusDesc.Type = GeometryType_Triangles;
	torusDesc.Flags = GeometryFlags_Opaque;
	torusDesc.Triangles.IndexCount = torus->GetIndexBuffer(0)->m_IndexCount;
	torusDesc.Triangles.StartIndexLocation = 0;
	torusDesc.Triangles.VertexCount = torus->GetVertexBuffer(0)->m_VertexCount;
	torusDesc.Triangles.StartVertexLocation = 0;
	torusDesc.Triangles.VertexPositionFormat = DataFormat_R32G32B32_FLOAT;
	
	torusDesc.Triangles.pIndexBuffer = torus->GetIndexBuffer(0);
	torusDesc.Triangles.pVertexBuffer = torus->GetVertexBuffer(0);

	BottomLevelAccelerationStructureDesc BlasDesc;
	BlasDesc.BufferOffset = 0;
	BlasDesc.NumGeometries = 1;
	BlasDesc.Geometries = &torusDesc;

	//Query struct size
	RaytracingASSizeInfo BlasSize = GraphicsContext::QueryAccelerationStructureSizeReq(BlasDesc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAG_PREFER_FAST_TRACE);

	TopLevelAccelerationStructureDesc TlasDesc;
	TlasDesc.NumInstances = 1;

	//figure out the storage size Note: Only required property for DX12 is the Number of instnaces
	RaytracingASSizeInfo TlasSize = GraphicsContext::QueryAccelerationStructureSizeReq(TlasDesc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAG_PREFER_FAST_TRACE);

	//create scratch first we are going to use the same scratch for both structures
	Scratch = ResourceFactory::CreateBuffer({ ResourceAccessFlag_GpuWrite | ResourceAccessFlag_GpuRead, BufferFlags_RaytacingStructures, ROUND_UP(std::max(BlasSize.ScratchSizeBuild, TlasSize.ScratchSizeBuild),GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT)});
//	DEBUG_NAME(Scratch, L"ScratchBuffer")

	//create storage buffer, it will be saved in TLAS and BLAS handles we create in Tutorial scene
	BufferRef StorageBuffer = ResourceFactory::CreateBuffer({ ResourceAccessFlag_GpuWrite | ResourceAccessFlag_GpuRead, BufferFlags_RaytacingStructures,ROUND_UP(BlasSize.StorageSize + TlasSize.StorageSize,GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT) });
//	DEBUG_NAME(StorageBuffer, L"RaytracingStructBuffer")

	//Acceleration Structures need to be in ACCESS_UAV | ACCESS_RAYTRACING_STRUCT
	BufferBarrier toUAV;
	toUAV.SyncBefore = SKTBD_SYNC_ALL;
	toUAV.SyncAfter = SKTBD_SYNC_ALL;
	toUAV.AccessBefore = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ;
	toUAV.AccessAfter = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_UNORDERED_ACCESS;
	std::array<BufferBarrier, 2> group{ toUAV,toUAV };
	group[0].Resource = Scratch.get();
	group[1].Resource = StorageBuffer.get();

	BarrierGroup ASbuffersGroup(group.data(),2);
	RenderCommand::Barrier(&ASbuffersGroup, 1);

	//Build Acceleration Structures, They both gonna be stored in the same buffer and can be built concurrectly
	BLAS = RenderCommand::BuildBottomLevelAS(BlasDesc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAG_PREFER_FAST_TRACE, StorageBuffer, Scratch.get(), 0);

	//TLAS stores instances of Blases an those have to be written into a buffer;
	InstanceData = ResourceFactory::CreateBuffer({ ResourceAccessFlag_CpuWrite,BufferFlags_NONE, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT });

	TlasDesc.BufferOffset = ROUND_UP(BlasSize.StorageSize, GraphicsConstants::RAYTRACING_STRUCT_ALIGNMENT);
	TlasDesc.FirstInstanceOffset = 0;
	TlasDesc.InstanceDataBuffer = InstanceData.get();

	//write instance information
	TLASInstanceData Instance{glm::translate(float3(0, -1, 0)) * glm::eulerAngleXYZ(glm::radians(90.f), glm::radians(0.f), glm::radians(0.f)),0,1,0,TLASInstanceFlags_::SKTBD_INSTANCE_FLAG_NONE, BLAS.GetHandle()};
	GraphicsContext::WriteTopLevelASInstanceDataToBuffer(InstanceData.get(), 0, 1, &Instance);

	//We need to stop building the bottom level acceleration structure before we can proceed build top level one
	BufferBarrier FlushCompute{};
	FlushCompute.SyncBefore = SKTBD_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE;
	FlushCompute.SyncAfter = SKTBD_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE;
	FlushCompute.AccessBefore = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_UNORDERED_ACCESS;
	FlushCompute.AccessAfter = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ;
	FlushCompute.Resource = StorageBuffer.get();

	BarrierGroup Flush(&FlushCompute);
	RenderCommand::Barrier(&Flush, 1);

	//Build top Level Acceleration structure
	TLAS = RenderCommand::BuildTopLevelAS(TlasDesc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAG_PREFER_FAST_TRACE, StorageBuffer, Scratch.get(), 0);

	//Last barrier, before we begin rendering we need to stop building any acceleration structures
	FlushCompute.SyncBefore = SKTBD_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE;
	FlushCompute.SyncAfter = SKTBD_SYNC_RAYTRACING;
	FlushCompute.AccessBefore = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE | SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ;
	FlushCompute.AccessAfter = SKTBD_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ;

	Flush = BarrierGroup(&FlushCompute);
	RenderCommand::Barrier(&Flush, 1);

	//DX12 can only do TLAS Views;
	ASView = ResourceFactory::CreateAccelerationStructureView(TLAS);

	//PIPELINE

	RaytracingPipelineDesc RTDDesc{};
	RTDDesc.SetRaytracingLibrary(L"RaytracingLibrary", L"MyRaygenShader");
	RTDDesc.AddHitGroup(L"PrimaryGroup", nullptr, L"MyClosestHitShader", nullptr, RaytracingHitGroupType_Triangles);
	RTDDesc.AddMissShader(L"MyMissShader");
	RTDDesc.SetConfig(sizeof(float4), sizeof(float2), 1);

	PipelineDesc rtPipeDesc{ PipelineType_Ray, RootSignature, &RTDDesc };
	RaytracingPipeline = ResourceFactory::CreatePipelineState(rtPipeDesc);

	auto ShaderTableStorage = ResourceFactory::CreateBuffer({ ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead, BufferFlags_NONE, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT });

	//SHADER TABLES

	Raygen = GraphicsContext::BuildShaderTable(ShaderTableStorage, 0, RaytracingPipeline.get(), RTDDesc, ShaderTableGroup::Raygen);
	Hit = GraphicsContext::BuildShaderTable(ShaderTableStorage, ROUND_UP(Raygen.Offset + Raygen.Size, GraphicsConstants::RAYTRACING_SHADER_TABLE_ALIGNMENT), RaytracingPipeline.get(), RTDDesc, ShaderTableGroup::Hit);
	Miss = GraphicsContext::BuildShaderTable(ShaderTableStorage, ROUND_UP(Hit.Offset + Hit.Size, GraphicsConstants::RAYTRACING_SHADER_TABLE_ALIGNMENT), RaytracingPipeline.get(), RTDDesc, ShaderTableGroup::Miss);

	//Output Texture
	TextureDesc desc{};
	desc.Width = GraphicsContext::GetClientWidth();
	desc.Height = GraphicsContext::GetClientHeight();
	desc.Depth = 1;
	desc.AccessFlags = ResourceAccessFlag_GpuRead | ResourceAccessFlag_GpuWrite;
	desc.Mips = 1;
	desc.Dimension = TextureDimension_Texture2D;
	desc.Type = TextureType_Default;
	desc.Format = DataFormat_DEFAULT_BACKBUFFER;

	OutputTexture.ForEach([&](auto& Texture) { Texture = ResourceFactory::CreateTextureBuffer(desc); });

	//create imgui output view
	TextureViewDesc srv{};
	srv.Dimension = TextureDimension_Texture2D;
	srv.ArraySize = 1;
	srv.Format = DataFormat_DEFAULT_BACKBUFFER;
	srv.MipLevels = 1;
	srv.MipSlice = 0;

	OutputUAV.ForEach([&, i = 0](auto& view) mutable -> void {view = ResourceFactory::CreateTextureUnorderedAccessView(srv, OutputTexture[i]); i++; });
	OutputSRV.ForEach([&, i = 0](auto& view) mutable -> void {view = ResourceFactory::CreateTextureShaderResourceView(srv, OutputTexture[i]); i++; });
	//constant data buffer for frame data
	ConstantData = ResourceFactory::CreateBuffer({ ResourceAccessFlag_CpuWrite | ResourceAccessFlag_GpuRead, BufferFlags_NONE, GraphicsConstants::DEFAULT_RESOURCE_ALIGNMENT });
	ConstantBufferViewDesc.ForEach([i=0](BufferViewDesc& desc)mutable->void {desc.InitAsConstantBuffer<RaytracingFrame>(GraphicsConstants::CONSTANT_BUFFER_ALIGNMENT * i); });
}

void Raytracer::Begin()
{
	RaytracingFrame data;

	auto aspect = GraphicsContext::GetClientAspectRatio();
	data.ProjectionMatrix = glm::perspectiveLH(glm::radians(90.f), aspect, 0.01f, 100.f);
	data.ViewMatrix = glm::lookAtLH(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	data.CameraMatrix = glm::inverse(data.ViewMatrix);

	data.AccelerationStructIndex = ASView->GetViewIndex();
	data.OutputUAVIndex = OutputUAV.Get()->GetViewIndex();

	GraphicsContext::CopyDataToBuffer(ConstantData.get(), ConstantBufferViewDesc.Get().Offset, sizeof(RaytracingFrame), &data);

	RenderCommand::SetInputLayoutCompute(RootSignature.get());
	RenderCommand::SetInlineResourceViewCompute(0, ConstantData.get(), ConstantBufferViewDesc.Get(), ViewAccessType_ConstantBuffer);

	RenderCommand::SetPipelineState(RaytracingPipeline.get());

}

void Raytracer::Trace()
{
	DispatchRaysDesc desc{};
	desc.RaygenRecord = &Raygen;
	desc.HitGroupTable = &Hit;
	desc.MissTable = &Miss;
	desc.Depth = 1;
	desc.Width = OutputTexture.Get()->GetWidth();
	desc.Height = OutputTexture.Get()->GetHeight();


	RenderCommand::DispatchRays(desc);
}

void Raytracer::End()
{
	//Move to next frame;
	++OutputSRV;
	++OutputUAV;
	++ConstantBufferViewDesc;
}

void Raytracer::RenderImGui()
{
	ImGui::Begin("Raytrace Result");

	ImGui::Image(OutputSRV.GetPrevious()->GetImTextureID(), ImVec2(OutputSRV.GetPrevious()->GetWidth(), OutputSRV.GetPrevious()->GetHeight()));

	ImGui::End();
}
