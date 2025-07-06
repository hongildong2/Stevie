#include "INCL_ShaderTypes.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstant;
}

cbuffer MeshConstant : register(b1)
{
	MeshConstant meshConstant;
}

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
	SamplingPixelShaderInput output;

	output.positionModel = input.positionModel;
	output.texcoord = input.texcoord;
	float4 pos = mul(float4(input.positionModel, 1.f), meshConstant.world);
	

	#ifdef DEPTH_ONLY
	float4 viewPos = mul(pos, depthView);
	output.positionProj = mul(viewPos, depthProj);
	#elif SKY_BOX
	float4 viewPos = mul(float4(input.positionModel, 0), view);
	output.positionProj = mul(float4(viewPos.xyz, 1), proj);
	#else
	float4 viewPos = mul(pos, globalConstant.view);
	output.positionProj = mul(viewPos, globalConstant.proj);
	#endif
	return output;
}