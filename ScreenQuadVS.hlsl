#include "RenderingCommons.hlsli"

#include "ScreenSpace.hlsli"


SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;
    
	output.position = float4(input.position, 1.0);
	output.texcoord = input.texcoord;

	return output;
}
