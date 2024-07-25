#include "Common.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};

Texture2D<float> displacementMap : register(t0);
Texture2D<float4> normalMap : register(t1);

SamplerState linearClamp : register(s0);

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};

#define NUM_CONTROL_POINTS 4

[domain("quad")]
PixelShaderInput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation, // � ��ǥ��ε��´ٴ°ž� ?����
	const OutputPatch<PixelShaderInput, NUM_CONTROL_POINTS> patch)
{
	PixelShaderInput Output;
	
	PixelShaderInput topLeft = patch[0]; // has tex coord (0,0)
	PixelShaderInput topRight = patch[1];
	PixelShaderInput bottomRight = patch[2]; // has tex coord (1,1)
	PixelShaderInput bottomLeft = patch[3]; 

	Output.texcoordinate = domain; // �̷��� �Ǵ°ǰ�??
	
	
	float3 topX = lerp(topLeft.positionModel, topRight.positionModel, domain.x);
	float3 bottomX = lerp(bottomLeft.positionModel, bottomRight.positionModel, domain.x);
	
	float3 res = lerp(topX, bottomX, domain.y);
	
	Output.positionModel = res;
	
	
	// TODO : multisampling?
	// normal model�� �ؽ��� ����
	float3 normalModel = normalMap.SampleLevel(linearClamp, Output.texcoordinate, 0.0).xyz;
	
	// displacement ���� �׳� ���ø��ؾ��ϳ� ��ֹ������� �ؾ��ϳ�?
	// ����ǥ�迡 displacement���� �ؾ��ϴ°�? ������ǥ�迡?
	// �ϴ� ����ǥ�迡 displacement �� ����, �Ķ���� �ٿ��� ���������ϰ� �������
	Output.positionModel.y = displacementMap.SampleLevel(linearClamp, Output.texcoordinate, 0.0);
	
	Output.normalWorld = mul(float4(normalModel, 0.f), worldIT).xyz;
	
	Output.positionWorld = mul(float4(Output.positionModel.xyz, 1.f), world).xyz;
	
	Output.positionProjection = mul(float4(Output.positionWorld, 1.f), view);
	Output.positionProjection = mul(Output.positionProjection, proj);
	
	
	// TODO : need to calculate
	Output.tangentWorld = topLeft.tangentWorld;
	
	return Output;
}
