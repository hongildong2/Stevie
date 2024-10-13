#include "RenderingCommons.hlsli"
#include "ScreenSpace.hlsli"

SamplingVertexShaderOutput main(SamplingVertexShaderInput input)
{
	SamplingVertexShaderOutput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	

	float4 viewPos = mul(float4(input.positionModel, 0), view);
	output.positionProj = mul(float4(viewPos.xyz, 1), proj);

	return output;
}