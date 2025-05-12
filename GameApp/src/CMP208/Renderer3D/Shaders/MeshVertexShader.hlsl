
#define __HLSL__ 1
#include "Renderer3DCommonStructs.h"

StructuredBuffer<InstanceData> Instances : register(t0, space0);

cbuffer FRAMEDATA : register(b0, space0)
{
    FrameData Frame;
}

static float2 Quad[6] =
{
		float2(0, 0),
		float2(0, -1),
		float2(1, -1),
		float2(1, -1),
		float2(1, 0),
		float2(0, 0)
};

static float2 QuadOffset[6] =
{
		float2(-0.5,  0.5)	,
		float2(-0.5, -0.5),
		float2(0.5, -0.5),
		float2(0.5, -0.5),
		float2(0.5, 0.5),
		float2(-0.5, 0.5)
};

// uint InstID : SV_StartInstanceLocation only supported on SM6.8

struct InstID
{
    uint StartInstanceLocation;
};

ConstantBuffer<InstID> DrawID : register(b1, space0);

VS_OUT main(VS_IN input)
{
    VS_OUT output;

    output.pos = float4(input.pos,1);
	output.uv = input.uv;
    output.normal = normalize(mul((float3x3)Instances[DrawID.StartInstanceLocation].m_transform, input.normal));
    output.tangent = normalize(mul((float3x3)Instances[DrawID.StartInstanceLocation].m_transform, input.tangent));
    output.InstID = DrawID.StartInstanceLocation;

    output.pos = mul(Instances[DrawID.StartInstanceLocation].m_transform, output.pos);
    output.worldpos = output.pos.xyz;
    output.pos = mul(Frame.View, output.pos);
    output.pos = mul(Frame.Projection, output.pos);

    return output;
}