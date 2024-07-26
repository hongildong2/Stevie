#include "OceanGlobal.hlsli"

RWTexture2D<float> HeightMap : register(u0);

SamplerState linearMirror : register(s0);

Texture2DArray<float4> DisplacementMap : register(t0);

StructuredBuffer<CombineParameter> parameters : register(t1);

cbuffer Params : register(b0)
{
	float simulationScale; // (m)
	bool bScale; // not in use
	float2 dummy;
};

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width, height;
	HeightMap.GetDimensions(width, height);
	
	float2 UV = float2(float(DTid.x) / float(width + 1), float(DTid.y) / float(height + 1));
	float3 displacement = MultiSampleDisplacementModel(DisplacementMap, parameters, linearMirror, CASCADE_COUNT, UV, simulationScale).xyz;
	
	// ���� �ùķ��̼� ũ�Ⱑ 512m x 512m ����� ��, Ƣ��� displacement�� �� ��ǥ��, �� �ִٰ� �����Ѵ�. ���� offset�� �ؽ�����ǥ��� �ٲ��ش�.
	

	
	HeightMap[DTid.xy] = displacement.y;
	

}


