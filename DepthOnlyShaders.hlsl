#include "RenderingCommons.hlsli"

float4 DepthOnlyVSMain(VertexShaderInput input) : SV_Position
{
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	float4 viewPos = mul(pos, view);
	return mul(viewPos, proj);
}