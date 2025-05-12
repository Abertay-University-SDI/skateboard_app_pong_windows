#pragma once
#include "Renderer3DCommonStructs.h"

#ifndef LIGHT_FUNCTIONS_H
#define LIGHT_FUNCTIONS_H

#define DIRECTIONAL 0
#define POINTLIGHT 1
#define SPOTLIGHT 2

#if defined(__PSSL__)

#define STRUCT_BUFFER(type) RegularBuffer<type>
#define SAMPLER sce::Agc::Core::Sampler

#elif defined (__HLSL__)

#define STRUCT_BUFFER(type) StructuredBuffer<type>
#define SAMPLER sampler

#endif

float3 ExpandNormal(float3 n)
{
    return (n * 2.0f) - 1.0f;
}

float3 DoNormalMapping(float3 input_tangent, float3 input_normal, Texture2D tex, SAMPLER s, float2 uv)
{
    //determine if we have a valid texture
    uint2 dim{};
    tex.GetDimensions(dim.x, dim.y);
    if(dim.x == 0 && dim.y == 0) return input_normal;

	//normal.xyz = normal.xyz * 2.0f - 1.0f;
	//normal.y = -normal.y;	// normal map has green channel inverted
	//float3 vBumpNorm = normalize(normal.x*input_tangent + normal.y*normalize(cross(input_normal,input_tangent)) + normal.z*input_normal);

    float3x3 TBN = float3x3(input_tangent, normalize(cross(input_normal, input_tangent)), input_normal);

    float3 normal = tex.Sample(s, uv).xyz;

    normal = ExpandNormal(normal);
    normal.y = -normal.y;

    // Transform normal from tangent space to view space. 
    normal = mul(normal,TBN);
	
    return normalize(normal);
}

float calculateSpotFalloffFactor(float alpha, Light light)
{
    float falloffFactor = pow((cos(alpha) - cos(light.OuterCone / 2)) / (cos(light.InnerCone / 2) - cos(light.OuterCone / 2)), light.FalloffPower);
    return falloffFactor;
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.

float calculateLightingIntensity(float3 normal, float3 lightvector)
{
    float intensity = saturate(dot(normal, normalize(lightvector)));
    return intensity;
}

//calculate specular lightting based on view angle Phong model

float calcSpecularIntensity(float SpecularPower, float3 inputView, float3 inputNormal, float3 lightVector)
{
    float3 halfway = normalize(lightVector + inputView);
    float specularIntensity = pow(max(dot(inputNormal, halfway), 0.f), SpecularPower);
    return specularIntensity;

}

float4 LetThereBelightSquared(float3 normal, float3 viewPos, float3 WorldPos, float shininessPow, uint LightCount, STRUCT_BUFFER(Light) Lights, float4 AmbientComponent)
{
    float4 LightComponent = float4(0, 0, 0, 0);
    float4 SpecularComponent = float4(0, 0, 0, 0);

    float3 viewVec = normalize(viewPos - WorldPos);


    if (LightCount > 0)
    {
       
        for (uint i = 0; i < LightCount; i++)
        {
            float3 lightDirection = -normalize(Lights[i].LightDirection);

            if (Lights[i].LightType == DIRECTIONAL)
            {
                LightComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calculateLightingIntensity(normal, lightDirection);
                SpecularComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calcSpecularIntensity(shininessPow, viewVec, normal, lightDirection);
            }

            if (Lights[i].LightType == POINTLIGHT)
            {
                float3 lightVector = Lights[i].LightPosition - WorldPos;
                float amplitude = length(lightVector);

                if (amplitude < Lights[i].CutOffDistance)
                {
                    float attune = 1.f / (Lights[i].ConstantAttenuation + Lights[i].LinearAttenuation * amplitude + Lights[i].SquareAttenuation * amplitude * amplitude);

                    //float3 lightVector = Lights[i].LightPosition - WorldPos;
                    //float amplitude = length(lightVector);

                    LightComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calculateLightingIntensity(normal, lightVector) * attune;
                    SpecularComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calcSpecularIntensity(shininessPow, viewVec, normal, lightVector) * attune;
                }
            }

            if (Lights[i].LightType == SPOTLIGHT)
            {
                float3 lightVector = Lights[i].LightPosition - WorldPos;
                float amplitude = length(lightVector);

                if (amplitude < Lights[i].CutOffDistance)
                {
                    //callculate attenuation // tip - very small values of attenuation can cause light to be yoinked beyond 2000 causing all white, amplifying the light
                    float attune = 1.f / (Lights[i].ConstantAttenuation + Lights[i].LinearAttenuation * amplitude + Lights[i].SquareAttenuation * amplitude * amplitude);

                    //float3 lightVector = Lights[i].LightPosition - WorldPos;
                    //float amplitude = length(lightVector);

                    float dot_p = dot(lightDirection, normalize(lightVector));
                    float angle = acos(dot_p);

                    if (angle < Lights[i].OuterCone / 2)
                    {
                        float falloff = calculateSpotFalloffFactor(angle, Lights[i]);
                        LightComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calculateLightingIntensity(normal, lightVector) * attune * falloff;
                        SpecularComponent += Lights[i].DiffuseColour * Lights[i].DiffuseColour * calcSpecularIntensity(shininessPow, viewVec, normal, lightVector) * attune * falloff;
                    }
                }
            }
        };

        //LightComponent /= (1.f + LightComponent);
        //SpecularComponent /= (1.f + SpecularComponent);

    }

    //return saturate(float4(AmbientComponent * AmbientComponent, 1) + LightComponent + SpecularComponent);
    return LightComponent + SpecularComponent + pow(AmbientComponent,2);

}

#endif
