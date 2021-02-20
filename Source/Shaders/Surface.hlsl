#ifndef SURFACE_INCLUDED
#define SURFACE_INCLUDED

#include "ConstantBuffers.hlsl"
#include "NormalMapping.hlsl"

#define ALPHA_TEST_THRESHOLD 0.5

Texture2D _BaseTexture             : register(t0);
Texture2D _NormalRoughMetalTexture : register(t1);
SamplerState _Sampler              : register(s0);

struct SurfaceOutput
{
	float3 albedo;
	float opacity;
	float3 normal;
	float roughness;
	float metalness;
};

SurfaceOutput Surface(float3 pointToEye, float3 normal, float2 uv, float4 vertColor)
{
	SurfaceOutput s;

	// Albedo + Opacity
	float4 baseTextureSample = _BaseTexture.Sample(_Sampler, uv);
	s.opacity = baseTextureSample.a;
#if ALPHA_TEST_ON
	clip(s.opacity - ALPHA_TEST_THRESHOLD);
#endif
	s.albedo = baseTextureSample.rgb * vertColor.rgb;

	// Normal
	float4 normalRoughMetal = _NormalRoughMetalTexture.Sample(_Sampler, uv);
	s.normal = perturb_normal(normal, float3(normalRoughMetal.xy, 0), pointToEye, uv);

	// Metalness
	s.metalness = normalRoughMetal.z;

	// Roughness
	s.roughness = normalRoughMetal.w;

	return s;
}

#endif