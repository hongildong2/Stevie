#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "ShaderTypes.hlsli"


/* Definitions */

/* Texture Sampler State */

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);
SamplerState shadowPointSampler : register(s2);
SamplerComparisonState shadowCompareSampler : register(s3);

/* Resources */

cbuffer GlobalConstants : register(b0)
{
	matrix view;
	matrix proj;
	matrix viewProj;
	
	matrix invView;
	matrix invProj;
	matrix invViewProj; // Projection Space to World
	
	float3 eyeWorld;
	float globalTime;
	
	float3 eyeDir;
	uint globalLightsCount;
	
	float nearZ;
	float farZ;
	float2 gcDummy;
};

cbuffer MeshConstants : register(b1)
{
	matrix world;
	matrix worldIT;
	matrix worldInv;
}

cbuffer MaterialConstants : register(b2)
{
	Material materialConstant;
};

cbuffer DepthMapConstant : register(b3)
{
	matrix depthView;
	matrix depthProj;
};

// cbuffer 5 ~ Not Reserved
/* 
	Textures 

	0 ~ 9 IBL Textures, t4 is GlobalLights Structured Buffer
	10 ~ 29 DepthMap, ShadowMap Textures
	30 ~ 59 Mesh Textures
	60 ~ 99 Environment Textures
	100 ~  Misc
*/




TextureCube cubeMap : register(t0);
TextureCube irradianceMap : register(t1);
TextureCube SpecularMap : register(t2);
Texture2D BRDFMap : register(t3);
StructuredBuffer<Light> globalLights : register(t4);

Texture2D<float> cameraDepthMap : register(t10);
Texture2D shadowMaps[SUN_LIGHT_INDEX + 1] : register(t11);

Texture2D<float3> albedoTex : register(t30);
Texture2D<float> aoTex : register(t31);
Texture2D<float> heightTex : register(t32);
Texture2D<float> metallicTex : register(t33);
Texture2D<float3> normalTex : register(t34);
Texture2D<float> roughnessTex : register(t35);
Texture2D<float> emissiveTex : register(t36);
Texture2D<float> opacityTex : register(t37);
// StructuredBuffer<Light> meshLights : register(t36);

#endif // __COMMON_HLSLI__