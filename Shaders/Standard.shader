//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------

#include "Surface.hlsl"
#include "Lighting.hlsl"

//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------

struct VSInputStandard
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct VSOutputStandardForward
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
};

struct VSOutputStandardShadow
{
	float4 position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Shader functions
//--------------------------------------------------------------------------------------

// Forward Pass

VSOutputStandardForward StandardForwardVS(VSInputStandard v)
{
	VSOutputStandardForward o;

	float4 worldPos = mul(_World, v.position);
	float4 viewPos = mul(_View, worldPos);
	float4 clipPos = mul(_Projection, viewPos);
	o.position = clipPos;

	o.normal = normalize(mul(_World, float4(v.normal, 0)).xyz);
	o.tangent = normalize(mul(_World, float4(v.tangent.xyz, 0)).xyz);
	o.binormal = cross(o.tangent, o.normal) * v.tangent.w;

	o.color = v.color;

	o.uv = v.uv;

	o.worldPos = worldPos.xyz;

	return o;
}

float4 StandardOpaqueForwardPS(VSOutputStandardForward i) : SV_TARGET
{
	float4 c = 1;

	SurfaceOutput s = Surface(i.normal, i.tangent, i.binormal, i.uv, i.color);

	c.rgb = Lighting(s, i.worldPos);
	
	return c;
}

// Shadow Pass

VSOutputStandardShadow StandardShadowVS(VSInputStandard v)
{
	VSOutputStandardShadow o;

	float4 worldPos = mul(_World, v.position);
	float4 viewPos = mul(_View, worldPos);
	float4 clipPos = mul(_Projection, viewPos);
	o.position = clipPos;

	return o;
}

void StandardOpaqueShadowPS()
{
}