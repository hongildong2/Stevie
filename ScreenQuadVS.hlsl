#include "RenderingCommons.hlsli"
#include "ScreenSpace.hlsli"


SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;
	
	// for full screen quad draw
	output.positionProj = float4(input.positionModel, 1);
	output.positionModel = input.positionModel; // screenquadVS
	output.texcoord = input.texcoord;
	
	return output;
}