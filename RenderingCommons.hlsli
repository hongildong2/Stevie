#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__


/* Definitions */
struct VertexShaderInput
{
	float3 positionModel : POSITION; //모델 좌표계의 위치 position
	float3 normalModel : NORMAL; // 모델 좌표계의 normal    
	float2 texcoordinate : TEXCOORD;
	float3 tangentModel : TANGENT;
};

struct PixelShaderInput
{
	float4 positionProjection : SV_POSITION; // Screen position
	float3 positionWorld : POSITION0; // World position (조명 계산에 사용)
	float3 normalWorld : NORMAL0;
	float2 texcoordinate : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 positionModel : POSITION1; // Volume casting 시작점
};

struct Light
{
	float3 radiance; // == strength
	float fallOffStart;
	
	float3 direction;
	float fallOffEnd;
	
	float3 positionWorld;
	float spotPower;
	
	float3 color;
	float dummy;
	
	uint type;
	float radius;
	float haloRadius;
	float haloStrength;
	
	matrix proj;
	matrix viewProj;
	matrix invProj;
};

struct Material
{
	float metallicFactor;
	float aoFactor;
	float roughnessFactor;
	float t1;
	
	bool bUseTexture;
	float3 albedo;
	
	float metallic;
	float roughness;
	float specular;
	float dummy;
};


/* Texture Sampler State */

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);

// not yet added
SamplerState shadowPointSampler : register(s2);
SamplerComparisonState shadowCompareSampler : register(s3);
SamplerState pointWrapSampler : register(s4);
SamplerState linearMirrorSampler : register(s5);
SamplerState pointClampSampler : register(s6);

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
	
	uint globalLightsCount; // sunLight not count, sunlight at index(count) of light SB;
	float nearZ;
	float farZ;
	float gcDummy;
};

uint getSunlightIndex()
{
	return globalLightsCount;
}

cbuffer MeshConstants : register(b1)
{
	matrix world;
	matrix worldIT;
	matrix worldInv;
	
	bool bUseHeightMap;
	float heightScale;
	uint meshLightsCount;
	float mcDummy;
}

cbuffer MaterialConstants : register(b2)
{
	Material materialConstant;
};


/* 
	Textures 

	0 ~ 9 IBL Textures, t4 is GlobalLights Structured Buffer
	10 ~ 19 DepthMap Textures
	20 ~ 29 ShadowMap Textures
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

Texture2DArray<float> globalShadowMap : register(t20); // For each global light, == light shadow map

Texture2D<float3> albedoTex : register(t30);
Texture2D<float> aoTex : register(t31);
Texture2D<float> heightTex : register(t32);
Texture2D<float> metallicTex : register(t33);
Texture2D<float3> normalTex : register(t34);
Texture2D<float> roughnessTex : register(t35);
// StructuredBuffer<Light> meshLights : register(t36);

static const float PI = 3.14159265359;

#endif // __COMMON_HLSLI__