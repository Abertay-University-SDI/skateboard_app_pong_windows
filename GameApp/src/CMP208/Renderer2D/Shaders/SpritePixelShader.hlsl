struct SpriteInstanceData
{
    uint m_SpriteTextureID;

    float4 m_Colour;

    float2 m_Size;
    float2 m_Offset;

    float2 m_UVScale;
    float2 m_UVOffset;

    matrix m_Transform;
};

struct FrameData
{
    matrix View;
    matrix Projection;
    matrix Camera;

    float4 AmbientLight;
    uint LightCount;
    uint LightsBuffer;
};

StructuredBuffer<SpriteInstanceData> Instances : register(t0, space0);

cbuffer FRAMEDATA : register(b0, space0)
{
    FrameData Frame;
}

SamplerState Sampler : register(s0);

static float4 TestColors[6] =
{
    float4(1, 0, 0, 1),
    float4(0, 1, 0, 1),
    float4(0, 0, 1, 1),
    float4(1, 1, 0, 1),
    float4(1, 1, 1, 1),
    float4(0, 1, 1, 1)
};

struct Input
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
    uint instID : SV_InstanceID;
};

uint4 unpackUINTColour(uint c)
{
    return uint4((c & 0xFF000000) >> 24, (c & 0xFF0000) >> 16, (c & 0xFF00) >> 8, c & 0xFF);
}

float4 main(Input input) : SV_TARGET
{
    //return float4(input.UV, 0, 1);
    //Instances[input.instID].m_Colour;

    Texture2D texture = ResourceDescriptorHeap[NonUniformResourceIndex(Instances[input.instID].m_SpriteTextureID)];

    if (Instances[input.instID].m_Colour.a < 0.f)
    {
        float alpha = step(distance(input.UV, float2(0.5, 0.5)), 0.5f);
        return float4(saturate(texture.Sample(Sampler, input.UV) + Instances[input.instID].m_Colour).xyz, alpha * (abs(Instances[input.instID].m_Colour.a) + texture.Sample(Sampler, input.UV).a));
        //return float4(saturate(texture.Sample(Sampler, input.UV) + Instances[input.instID].m_Colour).xyz, alpha);
    }
    
	return saturate(texture.Sample(Sampler, input.UV) + Instances[input.instID].m_Colour);

  

	
    //return saturate(texture.Sample(Sampler, input.UV));
}