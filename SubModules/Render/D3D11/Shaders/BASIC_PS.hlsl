#pragma enable_d3d11_debug_symbols

#include "INCL_ShaderTypes.hlsli"
#include "INCL_ShadowFunctions.hlsli"
#include "INCL_PBRFunctions.hlsli"
#include "INCL_CoordinateTransform.hlsli"

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);

TextureCube irradianceMap : register(t0);
TextureCube SpecularMap : register(t1);
Texture2D BRDFMap : register(t2);
StructuredBuffer<Light> SceneLights : register(t3);

Texture2D<float3> albedoTex : register(t4);
Texture2D<float> aoTex : register(t5);
Texture2D<float> metallicTex : register(t6);
Texture2D<float3> normalTex : register(t7);
Texture2D<float> roughnessTex : register(t8);
Texture2D<float> emissiveTex : register(t9);
Texture2D<float> opacityTex : register(t10);

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
	Light sunLightConstant;
};




float4 main(PixelShaderInput input) : SV_TARGET
{
	float viewDist = distance(globalConstants.eyeWorld, input.positionWorld);
	
	float3 albedo = materialConstant.bUseAlbedoTexture ? albedoTex.Sample(linearWrap, input.texcoordinate).rgb : materialConstant.albedo;
	float ao = materialConstant.bUseAOTexture ? aoTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.aoFactor : 1;
	float metallic = materialConstant.bUseMetallicTexture ? metallicTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.metallicFactor : materialConstant.metallic;
	float roughness = materialConstant.bUseRoughnessTexture ? roughnessTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.roughnessFactor : materialConstant.roughness;
	float3 emission = materialConstant.bUseEmissiveTexture ? emissiveTex.Sample(linearWrap, input.texcoordinate) : 0.0;
	
	float3 N = normalize(input.normalWorld);
	if (materialConstant.bUseNormalTexture)
	{
		N = GetNormalTexture(normalTex, linearWrap, false, N, input.tangentWorld, input.texcoordinate);
	}

	float3 V = normalize(globalConstants.eyeWorld - input.positionWorld);
	
	
	float3 F0 = float3(0.08, 0.08, 0.08) * materialConstant.specular; // specular 1 == 8%
	F0 = lerp(F0, albedo, metallic);
	
	
	
	// TODO :: Add SunLightRadiance, DynamicLightRadiance function into PBRFunctions.hlsli
	float3 Lo = float3(0.0, 0.0, 0.0);
	float shadowFactor = 1.0;
	// Sun Light
	Lo += RadianceByLight(sunLightConstant, F0, N, V, input.positionWorld, albedo, roughness, metallic) * shadowFactor;
	// ShadowFactor by sunlight
	
	// Dynamic Lights
	{
		//ShadowInput directShadowIn =
		//{
		//	shadowMaps[lightIndex], globalLights[lightIndex], input.positionWorld, shadowPointSampler, shadowCompareSampler, globalConstants.nearZ
		//};
		for (uint i = 0; i < globalConstants.globalLightsCount; ++i)
		{
			Lo += RadianceByLight(SceneLights[i], F0, N, V, input.positionWorld, albedo, roughness, metallic) * shadowFactor;
		
		}
	}
	
	
	// IBL	
	float3 ambientLight = 0;
	{
		float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
		float3 irradiance = irradianceMap.Sample(linearWrap, N).rgb;
		float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, reflect(-V, N), materialConstant.t1).rgb;
		float2 envBRDF = BRDFMap.Sample(linearClamp, float2(max(dot(N, V), 0.0), roughness)).xy;
	
		float3 kS = F;
		float3 kD = lerp(1.0 - kS, 0.0, metallic);
	
		float3 diffuse = irradiance * albedo;
		float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
		ambientLight = (kD * diffuse + specular) * ao;
	}
	
	
	float3 color = ambientLight * materialConstant.IBLStrength + Lo + emission * 10.0; // IBL + Lights
	
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1.0f);
}