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
	// height = height * 2.0 - 1.0;// ���̷����ұ�? ������, �� ������ 0 ~ 1 �Ǵ� 0 ~ 255�ϱ�, -1 ~ 1 �� ��ǥ�躯ȯ
	// output.positionWorld = output.positionWorld + 9.0 * height * output.normalWorld;
	
	output.positionProjection = mul(float4(output.positionWorld, 1.0), view);
	output.positionProjection = mul(output.positionProjection, proj);
	// ��� �������� world �� ���� height�� ����
	
	output.texcoordinate = input.texcoordinate;
	output.tangentWorld = mul(float4(input.tangentModel.xyz, 0.f), worldIT).xyz;
	output.positionModel = input.positionModel;
		
	return output;
}