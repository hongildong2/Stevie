#include "RenderingCommons.hlsli"
#include "ScreenSpace.hlsli"

SamplingVertexShaderOutput main(SamplingVertexShaderInput input)
{
	SamplingVertexShaderOutput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	

	float4 viewPos = mul(pos, depthView);
	output.positionProj = mul(viewPos, depthProj);
	return output;
}