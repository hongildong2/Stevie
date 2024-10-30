#ifndef __PBR_COMMONS__
#define __PBR_COMMONS__

#include "ScreenSpace.hlsli"
#include "ShaderTypes.hlsli"

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a2 = roughness * roughness;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	// float kForIBL = roughness * roughness / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return nom / denom;
}
  
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}


/*
	unreal says water specular is 0.255, which is in F0, 0.02;
		https://forums.unrealengine.com/t/pbr-specular-question/24078
		https://dev.epicgames.com/documentation/en-us/unreal-engine/physically-based-materials-in-unreal-engine
		Unreal Editor's Default Specular 0.5 == 0.04, 4% F0 here
		what is relation with IOR and Specular or Unreal?
		https://dev.epicgames.com/community/learning/tutorials/yG4z/unreal-engine-how-to-convert-index-of-refraction-ior-values-into-an-unreal-material-s-specular-value
*/
float GetUnrealSpecularFromIOR(float IOR)
{
	// eq : ((IOR - 1.0) / (IOR + 1.0))^2 / 0.08
	
	float val = (IOR - 1.0) / (IOR + 1.0);
	return val * val * 12.5;
}

float GetF0FromUnrealSpecular(float specular) // specular 1 = F0 0.08
{
	return 0.08 * specular;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 GetRadianceValue(Light light, float3 posWorld, float3 normalWorld)
{
	float3 L = light.positionWorld - posWorld;
	float lightDist = length(L);
	float spotFactor = 1;
	L /= lightDist;
	
	if (light.type == DIRECTIONAL_LIGHT)
	{
		L = -light.direction;
		lightDist = 1;
	}
	else if (light.type == SPOT_LIGHT)
	{
		spotFactor = pow(max(-dot(L, light.direction), 0.0f), light.spotPower);
	}
	
	float attenuation = saturate((light.fallOffEnd - lightDist) / (light.fallOffEnd - light.fallOffStart));
	
	return light.radiance * spotFactor * attenuation;
}

float3 RadianceByLight(Light light, float3 F0, float3 N, float3 V, float3 worldPos, float3 albedo, float roughness, float metallic)
{
	float3 L = normalize(light.positionWorld - worldPos);
	float3 H = normalize(V + L);
	
	float3 radiance = GetRadianceValue(light, worldPos, N);
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= 1.0 - metallic;
	
	float3 numerator = NDF * G * F;
	float denominator = max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 1e-5);
	float3 specular = numerator / denominator;
	
	float NdotL = max(dot(N, L), 0.0);
	
	
	// Why divide by PI?
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

#endif