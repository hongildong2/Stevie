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

	float3 N = normalize(input.normalWorld);
	float3 L = normalize(input.positionWorld - eyeWorld);

	return abs(dot(N, L)) * float4(0, 0, 1, 1);
}