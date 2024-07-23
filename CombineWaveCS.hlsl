#include "OceanGlobal.hlsli"

struct CombineParameter
{
	float L;
	float weight;
	float shoreModulation;
	float dummy;
};

RWTexture2D<float> HeightMap : register(u0);
RWTexture2D<float4> NormalMap : register(u1);

SamplerState wrapSampler : register(s0);

Texture2DArray<float4> DisplacementMap : register(t0);
Texture2DArray<float4> DerivativeMap : register(t1);
StructuredBuffer<CombineParameter> parameters : register(t2);

cbuffer Params : register(b0)
{
	float simulationScale; // (m)
	bool bScale; // not in use
	float2 dummy;
};


float3 SampleDisplacement(uint2 xzIndex, float2 offset)
{
	const float2 UV = float2(float(xzIndex.x) / float(SIZE), float(xzIndex.y) / float(SIZE));

	float3 displacement = 0;
	

	[unroll(CASCADE_COUNT)]
	for (uint cascade = 0; cascade < CASCADE_COUNT; ++cascade)
	{
		float factor = parameters[cascade].weight * parameters[cascade].shoreModulation;
		float uvScaler = min(1.0, simulationScale / parameters[cascade].L);
		
		float2 scaledOffset = offset * uvScaler; // �굵 �ؽ�����ǥ���..
		float2 scaledUV = float2(uvScaler * UV.x, uvScaler * UV.y);
		float valueScaler = simulationScale / parameters[cascade].L;
		
		float3 sampledDisplacement = valueScaler * DisplacementMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0).xyz;
		
		
		displacement += factor * sampledDisplacement;
	}

	return displacement; // �� ������ m����?
}

float3 GetNormalFromDerivative(uint2 xzIndex)
{
	const float2 UV = float2(float(xzIndex.x) / float(SIZE), float(xzIndex.y) / float(SIZE));

	float4 derivative = 0.0;
	
	[unroll(CASCADE_COUNT)]
	for (uint cascade = 0; cascade < CASCADE_COUNT; ++cascade)
	{
		float uvScaler = min(1.0, simulationScale / parameters[cascade].L);
		float2 scaledUV = float2(uvScaler * UV.x, uvScaler * UV.y);
		
		float valueScaler = simulationScale / parameters[cascade].L;
		float4 sampledDerivative = DerivativeMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0);
		
		sampledDerivative.z *= valueScaler; // TODO �� z�κи� �����ϸ� ������ �ִ��� �����غ���!!
		
		derivative += parameters[cascade].weight * sampledDerivative;
	}
	
	float2 slope = float2(derivative.x / max(0.001, 1 + derivative.z), derivative.y / max(0.001, 1 + derivative.w));
	
	return normalize(float3(-slope.x, 1, -slope.y));
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const uint2 XZ_INDEX = DTid.xy;
	
	float3 displacement = SampleDisplacement(XZ_INDEX, float2(0.0, 0.0));
	displacement = SampleDisplacement(XZ_INDEX, displacement.xz);
	displacement = SampleDisplacement(XZ_INDEX, displacement.xz);
	displacement = SampleDisplacement(XZ_INDEX, displacement.xz);
	// => ��Ƽ���ø�
	HeightMap[DTid.xy] = displacement.y;
	NormalMap[DTid.xy] = float4(GetNormalFromDerivative(XZ_INDEX), 0.0); // only xyz is meaningful, UAV does not support float3
	
	
	// TODO : ���δٸ� �������� �����͸� ��� �ϳ��� ��ĥ���ΰ�? �ؽ����� ��ǥ�� ���� �����ϴٰ� �� �� �ֳ�?
	// ���� �������ϰ����� �簢���� �������� ���ؾ��Ѵ�. 512m x 512m ũ���� �ٴٸ� �簢�� Quad�� ǥ���Ѵٰ� �ϸ�
	// 512x512 ���̸ʿ��� 1x1 �׸����� ũ��� 1mx1m �ϰ��̴�
	// �� cascade Displacement �׸����� ũ��� ���� L / 512 :: 2m, 0.5m, 0.03m, 0.005m �̷������� �ɰ��̴�
	// ���� ��ǥ�踦 Normalize�ؼ� �����ϰ��ؾ��Ѵ�! => �ؽ�����ǥ��� �����ؼ� ���÷��� ����
	// ������ �ε����� Quad�� ũ��� ������ -> HeightMap�� �Է��� �ؽ��� ��ǥ �����.
	// ��� �Ķ����  scaler = 1 / (Cascade.L /  OceanQuadSize) �� �ؽ��� ��ǥ�� ���ؼ� �ٵ��ǽ����� ����������
	// scaler = min(1, scaler); ���� �ʿ������� �𸣰����� �ؽ��ĸ� �� �����;.
	// �̷��� ���÷���, �ٵ����������� ���ø��ϴ� WRAP �Ǵ� MIRROR �ؽ��� ���÷����
}

