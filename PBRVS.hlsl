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



Texture2D<float> heightTex : register(t6);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	
	output.normalWorld = mul(float4(input.normalModel.xyz, 0.f), worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);
	output.positionWorld = mul(float4(input.positionModel.xyz, 1.f), world).xyz;
	
	
	// float height = heightTex.SampleLevel(linearWrap, input.texcoordinate, 0).r;
	// height = height * 2.0 - 1.0;// 왜이래야할까? 생각좀, 색 공간은 0 ~ 1 또는 0 ~ 255니까, -1 ~ 1 로 좌표계변환
	// output.positionWorld = output.positionWorld + 9.0 * height * output.normalWorld;
	
	output.positionProjection = mul(float4(output.positionWorld, 1.0), view);
	output.positionProjection = mul(output.positionProjection, proj);
	// 노멀 방향으로 world 에 대해 height맵 적용
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), worldIT).xyz;
	output.positionModel = input.positionModel;
		
	return output;
}