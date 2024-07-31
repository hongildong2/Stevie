#include "RenderingCommons.hlsli"
#include "CoordTransform.hlsli"


SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;
	
	// for full screen quad draw
	output.positionProj = float4(input.position, 1);
	output.positionModel = input.position; // screenquadVS
	output.texcoord = input.texcoord;
	
	return output;
}