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


float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = float4(0.1f, 0.9f, 0.1f, 1.f);
	float light = max(dot(dirLight.direction, input.normalWorld), 0.1f);
	color *= light;
	
	return color;
}