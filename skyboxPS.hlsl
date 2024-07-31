#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"

float4 main(SamplingPixelShaderInput input) : SV_TARGET0
{
	float3 color = cubeMap.Sample(linearClamp, input.positionModel.xyz);
	return float4(color, 1);
}