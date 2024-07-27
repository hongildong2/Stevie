#include "Common.hlsli"

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};


float4 DepthOnlyVSMain(VertexShaderInput input) : SV_Position
{
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	float4 viewPos = mul(pos, view);
	return mul(viewPos, proj);
}