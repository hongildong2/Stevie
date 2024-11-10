#include "ShaderTypes.hlsli"
#include "ScreenSpace.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstants;
}

static float4x4 IDENTITY_MATRIX =
{
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

SamplingVertexShaderOutput main(SamplingVertexShaderInput input)
{
	SamplingVertexShaderOutput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), IDENTITY_MATRIX);
	

	float4 viewPos = mul(float4(input.positionModel, 0), globalConstants.view);
	output.positionProj = mul(float4(viewPos.xyz, 1), globalConstants.proj);

	
	return output;
}