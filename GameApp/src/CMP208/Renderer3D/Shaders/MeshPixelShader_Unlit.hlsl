
#define __HLSL__ 1
#include "Renderer3DCommonStructs.h"
#include "LightFunctions.h"

StructuredBuffer<InstanceData> Instances : register(t0, space0);

cbuffer FRAMEDATA : register(b0, space0)
{
    FrameData Frame;
}

SamplerState Sampler : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
    //return Instances[input.instID].m_Colour;
    Texture2D texture = ResourceDescriptorHeap[NonUniformResourceIndex(Instances[input.InstID].AlbedoTextureID)];
	//return saturate(texture.Sample(Sampler, input.UV) + Instances[input.instID].m_Colour);
    return saturate(texture.Sample(Sampler, input.uv));
}