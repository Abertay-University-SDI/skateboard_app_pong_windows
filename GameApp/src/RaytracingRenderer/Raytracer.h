#pragma once
#include "Skateboard/Graphics/RHI/ResourceFactory.h"

using namespace Skateboard;

struct RaytracingFrame
{
	glm::mat4x4 ViewMatrix;
	glm::mat4x4 ProjectionMatrix;
	glm::mat4x4 CameraMatrix;

	uint32_t AccelerationStructIndex;
	uint32_t OutputUAVIndex;
};

class Raytracer
{
public:
	//Output Texture
	MultiResource<TextureBufferRef> OutputTexture;
	MultiResource<TextureSRVRef>  OutputSRV;
	MultiResource<TextureUAVRef> OutputUAV;

	//Keeping a reference alive to keep the buffer around builds
	Skateboard::BufferRef Scratch;

	//Blas Handle, blas will use the Geometry from the asset manager
	BottomLevelAccelerationStructure BLAS;

	//buffer for tlas instance data needs to stay alive for building
	BufferRef InstanceData;

	//Tlas handle
	TopLevelAccelerationStructure TLAS;

	//acceleration structure view passed accessed thorugh the shaders
	ASViewRef ASView;

	//Frame Constant buffers
	BufferRef ConstantData;
	MultiResource<BufferViewDesc> ConstantBufferViewDesc;

	//shadertables
	ShaderTable Raygen; // We only can pass one record but for convinience we have a whole table in one
	ShaderTable Miss;
	ShaderTable Hit;
	//ShaderTable Callable; we are not using callable shaders

	//Root signature, global signature needs to match in all the raytracing shaders
	ShaderInputLayoutRef RootSignature;

	//pipeline state
	PipelineRef RaytracingPipeline;

	virtual void Init();
	virtual void Begin();
	virtual void Trace();
	virtual void End();
	virtual void RenderImGui();
};

