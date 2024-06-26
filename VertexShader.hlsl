#include "common.hlsli"

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};

SamplerState linearWrap : register(s0);
SamplerState linearClamp : register(s1);



Texture2D heightTex : register(t6);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	
	output.normalWorld = mul(float4(input.normalModel.xyz, 0.f), worldIT).xyz;
	output.positionWorld = mul(float4(input.positionModel.xyz, 1.f), world).xyz;
	output.positionProjection = mul(float4(output.positionWorld, 1.f), view);
	output.positionProjection = mul(output.positionProjection, proj);
	// 노멀 방향으로 world 에 대해 height맵 적용
	
	float height = heightTex.SampleLevel(linearWrap, input.texcoordinate, 0).r;
	height = height * 2.0 - 1.0;// 왜이래야할까? 생각좀
	output.positionWorld = output.positionWorld + 9.0 * height * output.normalWorld;
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), worldIT).xyz;
	output.positionModel = input.positionModel;
	
	
	return output;
}