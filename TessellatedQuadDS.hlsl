#include "Common.hlsli"
#include "OceanGlobal.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4] : SV_TessFactor;
	float InsideTessFactor[2] : SV_InsideTessFactor;
};

Texture2DArray<float4> DisplacementMap : register(t0);
Texture2DArray<float4> DerivativeMap : register(t1);
StructuredBuffer<CombineParameter> parameters : register(t2);

SamplerState linearMirror : register(s0);

cbuffer transform : register(b0)
{
	float4x4 world;
	float4x4 worldIT;
	float4x4 view;
	float4x4 proj;
};

cbuffer Params : register(b1)
{
	float simulationScale; // (m)
	bool bScale; // not in use
	float2 dummy;
};



#define NUM_CONTROL_POINTS 4

float3 BilinearInterpolation(float2 uv, float3 topLeft, float3 topRight, float3 bottomRight, float3 bottomLeft)
{
	float3 topX = lerp(topLeft, topRight, uv.x);
	float3 bottomX = lerp(bottomLeft, bottomRight, uv.x);
	
	return lerp(topX, bottomX, uv.y);
}

[domain("quad")]
PixelShaderInput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<PixelShaderInput, NUM_CONTROL_POINTS> patch)
{
	PixelShaderInput Output;
	
	PixelShaderInput topLeft = patch[0]; // has tex coord (0,0)
	PixelShaderInput topRight = patch[1];
	PixelShaderInput bottomRight = patch[2]; // has tex coord (1,1)
	PixelShaderInput bottomLeft = patch[3]; 

	Output.normalWorld = topLeft.normalWorld; // 어차피 평면 quad
	Output.positionModel = BilinearInterpolation(domain, topLeft.positionModel, topRight.positionModel, bottomRight.positionModel, bottomLeft.positionModel);
	Output.texcoordinate = BilinearInterpolation(domain, float3(topLeft.texcoordinate, 0), float3(topRight.texcoordinate, 0), float3(bottomRight.texcoordinate, 0), float3(bottomLeft.texcoordinate, 0)).xy;
	
	
	// displacement 맵을 그냥 샘플링해야하나 노멀방향으로 해야하나?
	// 모델좌표계에 displacement맵을 해야하는가? 월드좌표계에?
	// 일단 모델좌표계에 displacement 맵 적용, 파라미터 붙여서 조절가능하게 상수버퍼
	
	float2 uvModel = Output.texcoordinate;
	float height = MultiSampleDisplacementModel(DisplacementMap, parameters, linearMirror, CASCADE_COUNT, uvModel, simulationScale).y;
	
	Output.positionWorld = mul(float4(Output.positionModel.xyz, 1.f), world).xyz;
	Output.positionWorld += 1.f * height * Output.normalWorld; // normal is not yet mapped, height mapping first
	
	float3 normalModel = SampleNormalModel(DerivativeMap, parameters, linearMirror, CASCADE_COUNT, uvModel, simulationScale);
	Output.normalWorld = mul(float4(normalModel, 0.f), worldIT).xyz;
	Output.normalWorld = normalize(Output.normalWorld);
	
	Output.positionProjection = mul(float4(Output.positionWorld, 1.f), view);
	Output.positionProjection = mul(Output.positionProjection, proj);
	
	
	// TODO : need to calculate
	Output.tangentWorld = mul(float4(topLeft.tangentWorld, 0.f), worldIT).xyz;
	
	return Output;
}
