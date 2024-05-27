#include "Common.hlsli"

cbuffer Lights : register(b1)
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
};

cbuffer PSConstant : register(b2)
{
	float3 eyeWorld;
	float dummy;
	float3 albedo;
	float metallic;
	float roughness;
	float ao;
	float t1;
	float t2;
};

static const float PI = 3.14159265359;

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

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 RadianceLByDirectLight(Light light, float3 F0, float3 N, float3 V, float3 worldPos)
{
	float3 L = normalize(light.position - worldPos);
	float3 H = normalize(V + L);
	
	float distance = length(light.position - worldPos);
	float attenuation = 1.0 / (distance * distance);
	float3 radiance = float3(1.0, 1.0, 1.0); // originally, lightColor * attenuation for pointlight
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= 1.0 - metallic;
	
	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	float3 specular = numerator / denominator;
	
	float NdotL = max(dot(N, L), 0.0);
	
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 N = normalize(input.normalWorld);
	float3 V = normalize(eyeWorld - input.positionWorld);
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	// for light
	
	Lo += RadianceLByDirectLight(dirLight, F0, N, V, input.positionWorld);
	
	float3 ambient = albedo * ao * t1; // diffuse term?
	float3 color = ambient + Lo;
	
	color = color / (color + float3(1.0, 1.0, 1.0)); // ??

	return float4(color, 1.0f);
}