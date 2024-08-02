#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), world);
	

	#ifdef DEPTH_ONLY
	float4 viewPos = mul(pos, depthView);
	output.positionProj = mul(viewPos, depthProj);
	#else
	float4 viewPos = mul(pos, view);
	output.positionProj = mul(viewPos, proj);
	#endif
	return output;
}