#include "common.hlsli"

cbuffer Lights : register(b1)
{
	Light dirLight;
	Light pointLight;
	Light spotLight;
};

cbuffer PSConstant : register(b2)
{
	float3 eyeWorld;
	bool useTexture;
	Material material;
	float3 rimColor;
	float rimPower;
	float rimStrength;
	bool useSmoothStep;
	float2 dummy;
};

TextureCube cubeMap : register(t0);
Texture2D albedo : register(t1);

SamplerState linearSampler : register(s0);



float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 lightColor = ComputeDirectionalLight(dirLight, material, input.normalWorld, normalize(eyeWorld - input.positionWorld));
	float3 toCube = reflect(-normalize(eyeWorld - input.positionWorld), input.normalWorld);
	
	float3 color = albedo.Sample(linearSampler, input.texcoordinate).rgb;
	float3 cube = cubeMap.Sample(linearSampler, toCube).rgb;
	return float4(lightColor * (color + cube),1);

}