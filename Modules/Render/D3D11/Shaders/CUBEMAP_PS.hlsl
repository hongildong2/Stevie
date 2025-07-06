#include "INCL_ShaderTypes.hlsli"

TextureCube cubeMap : register(t0);
SamplerState linearClamp : register(s0);

float4 main(SamplingPixelShaderInput input) : SV_TARGET0
{
	float3 color = cubeMap.Sample(linearClamp, input.positionModel.xyz);
	return float4(color, 1);
}