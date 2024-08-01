#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	

	float4 viewPos = mul(pos, view);
	output.positionProj = mul(viewPos, proj);
	return output;
}