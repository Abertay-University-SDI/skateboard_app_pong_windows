#pragma once

#ifndef RENDERER3D_COMMON_H_
#define RENDERER3D_COMMON_H_

#if defined(__cplusplus)
#include "Skateboard/Mathematics.h"
using namespace glm;
using namespace Skateboard;
#define SEMANTIC_POSITION ;
#define SEMANTIC(name) ;

#elif defined(__PSSL__)

#define SEMANTIC_POSITION : S_POSITION;
#define SEMANTIC(name) : name;

#elif defined(__HLSL__)

#define SEMANTIC_POSITION : SV_POSITION;
#define SEMANTIC(name) : name;

#endif

struct VS_IN
{
	float3 pos SEMANTIC(POSITION)
	float3 normal SEMANTIC(NORMAL)
	float3 tangent SEMANTIC(TANGENT)
	float2 uv SEMANTIC(TEXCOORD)
};

struct VS_OUT
{
	float4 pos SEMANTIC_POSITION
	//float4 colour SEMANTIC(COLOR)
	float3 normal SEMANTIC(NORMAL)
	float3 tangent SEMANTIC(TANGENT)
	float2 uv SEMANTIC(TEXCOORD)
	float3 worldpos SEMANTIC(WORLD)
	int InstID SEMANTIC(SV_InstanceID)
};

enum LightType : uint
{
	LightDirectional = 0,
	LightPoint = 1,
	LightSpot = 2,
};

struct FrameData
{
	matrix View;
	matrix Projection;
	matrix Camera;

	float4 AmbientLight;
	uint  LightCount;
	uint  LightsBuffer;
};

struct InstanceData
{
	matrix m_transform;

	float shininess;
	uint AlbedoTextureID;
	uint NormalTextureID;
};

struct Light
{
	float4 DiffuseColour;
	
	float ConstantAttenuation;
	float LinearAttenuation;
	float SquareAttenuation;
	float CutOffDistance;

	float3 LightPosition;
	float InnerCone;

	float3 LightDirection;
	float OuterCone;

	float FalloffPower;
	LightType LightType;

	//matrix LightProjection;
	//matrix LightView;
};

#endif
