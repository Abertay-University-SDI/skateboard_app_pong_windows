
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
	//return input.colour;

    //return Instances[input.instID].m_Colour;
    Texture2D texture = ResourceDescriptorHeap[NonUniformResourceIndex(Instances[input.InstID].AlbedoTextureID)];
    Texture2D normal = ResourceDescriptorHeap[NonUniformResourceIndex(Instances[input.InstID].NormalTextureID)];
	//return saturate(texture.Sample(Sampler, input.UV) + Instances[input.instID].m_Colour);

	//return normal.Sample(Sampler, input.uv);
    //return float4(input.tangent,1);
    //return float4(DoNormalMapping(input.tangent, input.normal, normal, Sampler, input.uv), 1);

    return saturate(sqrt(LetThereBelightSquared(DoNormalMapping(input.tangent, input.normal, normal, Sampler, input.uv), Frame.Camera._14_24_34, input.worldpos, Instances[input.InstID].shininess, Frame.LightCount, ResourceDescriptorHeap[Frame.LightsBuffer], Frame.AmbientLight) * pow(texture.Sample(Sampler, input.uv), 2)));
    //return saturate(sqrt(LetThereBelightSquared(input.normal, Frame.Camera._14_24_34, input.worldpos, Instances[input.InstID].shininess, Frame.LightCount, ResourceDescriptorHeap[Frame.LightsBuffer], float3(0, 0, 0)) * pow(texture.Sample(Sampler, input.uv), 2)));
}