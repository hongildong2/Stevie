#ifndef __SHADER_TYPES__
#define __SHADER_TYPES__

#define DIRECTIONAL_LIGHT (0)
#define SPOT_LIGHT (1)

struct VertexShaderInput
{
	float3 positionModel : POSITION; //모델 좌표계의 위치 position
	float2 texcoordinate : TEXCOORD;
	float3 normalModel : NORMAL; // 모델 좌표계의 normal    
	float3 tangentModel : TANGENT;
};

struct VertexShaderOutput
{
	float4 positionProjection : SV_POSITION; // Screen position
	float3 positionWorld : POSITION0; // World position (조명 계산에 사용)
	float3 normalWorld : NORMAL0;
	float2 texcoordinate : TEXCOORD0;
	float3 tangentWorld : TANGENT0;
	float3 positionModel : POSITION1; // Volume casting 시작점2
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

struct SamplingVertexShaderInput
{
	float3 positionModel : POSITION;
	float2 texcoord : TEXCOORD;
};



struct SamplingPixelShaderInput
{
	float3 positionModel : POSITION;
	float4 positionProj : SV_POSITION;
	float2 texcoord : TEXCOORD;
};


struct Light
{
	float3 direction;
	float radiance;
	
	float3 color;
	float fallOffStart;
	
	float3 positionWorld;
	float fallOffEnd;
	
	float spotPower;
	uint type;
	float radius;
	float dummy;
	
	matrix view;
	matrix proj;
	matrix invProj;
};

struct ShadowInput
{
	Texture2D shadowMap;
	Light light;
	float3 posWorld;
	SamplerState shadowPointSampler;
	SamplerComparisonState shadowCompareSampler;
	float nearZ;
};

struct MaterialConstant
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
	float IBLStrength;
		
	bool bUseHeightMap;
	float heightScale;
	float2 mcDummy;
	
	bool bUseAlbedoTexture;
	bool bUseAOTexture;
	bool bUseHeightTexture;
	bool bUseMetallicTexture;
	
	bool bUseNormalTexture;
	bool bUseRoughnessTexture;
	bool bUseEmissiveTexture;
	bool bUseOpacityTexture;
};

struct GlobalConstant
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
	uint shadwoingLightsCount;
	float gcDummy;
};

struct MeshConstant
{
	matrix world;
	matrix worldIT;
	matrix worldInv;
};

struct DepthConstant
{
	matrix viewProj;
};

static const float PI = 3.14159265359;


#endif