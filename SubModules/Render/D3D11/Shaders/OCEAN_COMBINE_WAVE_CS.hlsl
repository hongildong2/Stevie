#include "OceanGlobal.hlsli"

RWTexture2D<float> HeightMap : register(u0);

SamplerState linearWrapEE : register(s0);

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
	float2 UV = float2(float(DTid.x) / float(SIZE), float(DTid.y) / float(SIZE));
	OceanSamplingInput hi =
	{
		DisplacementMap, parameters, linearWrapEE, CASCADE_COUNT, UV, simulationScale
	};
	
	float3 displacement = MultiSampleDisplacementModel(hi).xyz;
	
	
	HeightMap[DTid.xy] = displacement.y;
}


