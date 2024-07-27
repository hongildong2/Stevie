#include "Common.hlsli"
#include "PBRCommons.hlsli"

cbuffer Lights : register(b1)
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
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

cbuffer PSConstant : register(b2)
{
	float3 eyeWorld;
	float dummy;
	Material materialConstant;
};


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


float4 main(PixelShaderInput input) : SV_TARGET
{

	float3 albedo;
	float ao;
	float metallic;
	float roughness;
	
	if (materialConstant.bUseTexture)
	{
		albedo = albedoTex.Sample(linearWrap, input.texcoordinate).rgb;
		ao = aoTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.aoFactor;
		metallic = metallicTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.metallicFactor;
		roughness = roughnessTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.roughnessFactor;
	}
	else
	{
		albedo = materialConstant.albedo;
		ao = 1.f;
		metallic = materialConstant.metallic;
		roughness = materialConstant.roughness;
	}
	
	
	float3 N = normalize(GetNormal(input));
	float3 V = normalize(eyeWorld - input.positionWorld);
	
	float3 F0 = float3(0.08, 0.08, 0.08) * materialConstant.specular;
	
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
	
	float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, reflect(-V, N), materialConstant.t1);
	float2 envBRDF = BRDFMap.Sample(linearClamp, float2(max(dot(N, V), 0.0), roughness));
	
	float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + Lo;
	
	// color = color / (color + float3(1.0, 1.0, 1.0)); // ??
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1.0f);
}