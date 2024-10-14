#ifndef __SHADER_TYPES__
#define __SHADER_TYPES__

#define SUN_LIGHT_INDEX (8)

#define DIRECTIONAL_LIGHT (0)
#define POINT_LIGHT (1)
#define SPOT_LIGHT (2)

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


struct Light
{
	float3 radiance;
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
	
	matrix view;
	matrix proj;
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

static const float PI = 3.14159265359;


#endif