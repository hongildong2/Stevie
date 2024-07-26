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
	
	// 현재 시뮬레이션 크기가 512m x 512m 라고할 때, 튀어나온 displacement도 그 좌표계, 에 있다고 봐야한다. 따라서 offset도 텍스쳐좌표계로 바꿔준다.
	

	
	HeightMap[DTid.xy] = displacement.y;
	

}


