#include "ShaderTypes.hlsli"
#include "ScreenSpace.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstants;
}

cbuffer MeshConstants : register(b1)
{
	MeshConstant meshConstants;
}

SamplingVertexShaderOutput main(SamplingVertexShaderInput input)
{
	SamplingVertexShaderOutput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), meshConstants.world);
	

	float4 viewPos = mul(float4(input.positionModel, 0), globalConstants.view);
	output.positionProj = mul(float4(viewPos.xyz, 1), globalConstants.proj);

	
	return output;
}