#include "INCL_ShaderTypes.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstants;
};

cbuffer MeshConstants : register(b1)
{
	MeshConstant meshConstants;
}

cbuffer DepthConstants : register(b2)
{
	DepthConstant depthConstants;
}

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), meshConstants.world);
	output.positionProj = mul(pos, depthConstants.viewProj);
	return output;
}