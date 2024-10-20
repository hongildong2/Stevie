#include "ShaderTypes.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstant;
}

cbuffer MeshConstant : register(b1)
{
	MeshConstant meshConstant;
}

cbuffer MaterialConstant : register(b2)
{
	MaterialConstant materialConstant;
}

SamplerState linearWrap : register(s0);

Texture2D<float> heightTex : register(t0);


VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	
	output.normalWorld = mul(float4(input.normalModel.xyz, 0.f), meshConstant.worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);
	output.positionWorld = mul(float4(input.positionModel.xyz, 1.f), meshConstant.world).xyz;
	

	if (materialConstant.bUseHeightMap)
	{
		float height = heightTex.SampleLevel(linearWrap, input.texcoordinate, 0).r;
		height = height * 2.0 - 1.0; // 왜이래야할까? 생각좀, 색 공간은 0 ~ 1 또는 0 ~ 255니까, -1 ~ 1 로 좌표계변환
		input.positionModel = input.positionModel + materialConstant.heightScale * height * input.normalModel;
	}
	
	 
	output.positionProjection = mul(float4(output.positionWorld, 1.0), globalConstant.view);
	output.positionProjection = mul(output.positionProjection, globalConstant.proj);
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), meshConstant.worldIT).xyz;
	output.positionModel = input.positionModel;
		
	return output;
}