
#ifndef RAYTRACINGHLSLCOMPAT_H
#define RAYTRACINGHLSLCOMPAT_H

#ifdef HLSL
typedef float3 vec3;
typedef float4 vec4;
typedef float4x4 matrix4x4;
typedef uint uint32_t;
#else

using namespace DirectX;
#include "Skateboard/Mathematics.h"
// Shader will use byte encoding to access indices.
typedef UINT16 Index;
#endif

struct SceneConstantBuffer
{
    matrix projectionToWorld;
    vec4 cameraPosition;
    vec4 lightPosition;
    vec4 lightAmbientColor;
    vec4 lightDiffuseColor;
};

struct CubeConstantBuffer
{
    vec4 albedo;
};

struct Vertex
{
    vec3 position;
    vec3 normal;
};

struct RayPayload
{
    float4 color;
};
#endif // RAYTRACINGHLSLCOMPAT_H