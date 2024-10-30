#include "ShaderTypes.hlsli"
#include "ScreenSpace.hlsli"
#include "PBRCommons.hlsli"

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);

TextureCube cubeMap : register(t0);
TextureCube irradianceMap : register(t1);
TextureCube SpecularMap : register(t2);
Texture2D BRDFMap : register(t3);

Texture2D<float3> albedoTex : register(t4);
Texture2D<float> aoTex : register(t5);
Texture2D<float> heightTex : register(t6);
Texture2D<float> metallicTex : register(t7);
Texture2D<float3> normalTex : register(t8);
Texture2D<float> roughnessTex : register(t9);
Texture2D<float> emissiveTex : register(t10);
Texture2D<float> opacityTex : register(t11);

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstants;
};

cbuffer MeshConstants : register(b1)
{
	MeshConstant meshConstants;
}

cbuffer MaterialConstants : register(b2)
{
	MaterialConstant materialConstant;
};

cbuffer SunLight : register(b3)
{
	Light lightConstant;
};

float3 GetNormal(PixelShaderInput input)
{
	float3 normalWorld = normalize(input.normalWorld);

	float3 normal = normalTex.Sample(linearWrap, input.texcoordinate).rgb;
	normal = 2.0 * normal - 1.0; // 범위 조절 [-1.0, 1.0]

    // OpenGL 용 노멀맵일 경우에는 y 방향을 뒤집어줍니다.
	// normal.y = invertNormalMapY ? -normal.y : normal.y;
        
	float3 N = normalWorld;
	float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
	float3 B = cross(N, T);
        
    // matrix는 float4x4, 여기서는 벡터 변환용이라서 3x3 사용
	float3x3 TBN = float3x3(T, B, N);
	normalWorld = normalize(mul(normal, TBN));
    
	return normalWorld;
}


float4 main(PixelShaderInput input) : SV_TARGET
{
	float viewDist = distance(globalConstants.eyeWorld, input.positionWorld);
	
	float3 albedo = materialConstant.bUseAlbedoTexture ? albedoTex.Sample(linearWrap, input.texcoordinate).rgb : materialConstant.albedo;
	float ao = materialConstant.bUseAOTexture ? aoTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.aoFactor : 1;
	float metallic = materialConstant.bUseMetallicTexture ? metallicTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.metallicFactor : materialConstant.metallic;
	float roughness = materialConstant.bUseRoughnessTexture ? roughnessTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.roughnessFactor : materialConstant.roughness;
	float3 emission = materialConstant.bUseEmissiveTexture ? emissiveTex.Sample(linearWrap, input.texcoordinate) : 0.0;
	
	float3 N = materialConstant.bUseNormalTexture ? GetNormal(input) : input.normalWorld;
	float3 V = normalize(globalConstants.eyeWorld - input.positionWorld);
	
	
	float3 F0 = float3(0.08, 0.08, 0.08) * materialConstant.specular; // specular 1 == 8%
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = float3(0.0, 0.0, 0.0);
	float shadowFactor = 1.0;
	Lo += RadianceByLight(lightConstant, F0, N, V, input.positionWorld, albedo, roughness, metallic) * shadowFactor;

	//ShadowInput directShadowIn =
	//{
	//	shadowMaps[lightIndex], globalLights[lightIndex], input.positionWorld, shadowPointSampler, shadowCompareSampler, globalConstants.nearZ
	//};
		
	
	
	// IBL	
	float3 ambient = 0;
	{
		float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
		float3 irradiance = irradianceMap.Sample(linearWrap, N).rgb;
		float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, reflect(-V, N), materialConstant.t1).rgb;
		float2 envBRDF = BRDFMap.Sample(linearClamp, float2(max(dot(N, V), 0.0), roughness)).xy;
	
		float3 kS = F;
		float3 kD = lerp(1.0 - kS, 0.0, metallic);
	
		float3 diffuse = irradiance * albedo;
		float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
		ambient = (kD * diffuse + specular) * ao;
	}
	
	
	float3 color = ambient * materialConstant.IBLStrength + Lo + emission * 10.0; // IBL + Lights
	
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1.0f);
}