
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

struct Out
{
    float4 position : SV_POSITION;
    float2 UV : TEXCOORD;
    uint instID : SV_InstanceID;
};


Out main(uint VertID : SV_VertexID, uint InstID : SV_InstanceID )
{
    Out output;

    output.position = float4(Quad[VertID] * Instances[InstID].m_Size + Instances[InstID].m_Offset, 0.f, 1.f);
    output.UV = abs(Quad[VertID]) * Instances[InstID].m_UVScale + Instances[InstID].m_UVOffset;

    output.position = mul(Instances[InstID].m_Transform, output.position);
    output.position = mul(Frame.View, output.position);
    output.position = mul(Frame.Projection, output.position);

    output.instID = InstID;

    return output;
}