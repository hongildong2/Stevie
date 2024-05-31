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
	float metallicFactor;
	float aoFactor;
	float roughnessFactor;
	float t1;
};

static const float PI = 3.14159265359;

TextureCube cubeMap : register(t0);
TextureCube irradianceMap : register(t1);
TextureCube SpecularMap : register(t2);
Texture2D BRDFMap : register(t3);


Texture2D albedoTex : register(t4);
Texture2D aoTex : register(t5);
Texture2D hegihtTex : register(t6);
Texture2D metallicTex : register(t7);
Texture2D normalTex : register(t8);
Texture2D roughnessTex : register(t9);

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);


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

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 RadianceLByDirectLight(Light light, float3 F0, float3 N, float3 V, float3 worldPos, float3 albedo, float roughness, float metallic)
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
	float3 albedo = albedoTex.Sample(linearWrap, input.texcoordinate).rgb;
	float ao = aoTex.Sample(linearWrap, input.texcoordinate).r * aoFactor;
	float metallic = metallicTex.Sample(linearWrap, input.texcoordinate).r * metallicFactor;
	float roughness = roughnessTex.Sample(linearWrap, input.texcoordinate).r * roughnessFactor;
	
	float3 N = normalize(GetNormal(input));
	float3 V = normalize(eyeWorld - input.positionWorld);
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	// Only One Directional Light added
	Lo += RadianceLByDirectLight(dirLight, F0, N, V, input.positionWorld, albedo, roughness, metallic);
	
	// IBL
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 kS = F;
	float3 kD = lerp(1.0 - kS, 0.0, metallic);
	
	float3 irradiance = irradianceMap.Sample(linearWrap, N);
	float3 diffuse = irradiance * albedo;
	
	float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, reflect(-V, N), t1);
	float2 envBRDF = BRDFMap.Sample(linearClamp, float2(max(dot(N, V), 0.0), roughness));
	
	float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + Lo;
	
	// color = color / (color + float3(1.0, 1.0, 1.0)); // ??
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1.0f);
}