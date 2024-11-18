#include "OceanGlobal.hlsli"

// Note : This texture need to be initialized to -5 at t =0
RWTexture2DArray<float4> TurbulencenessTex : register(u0);

Texture2DArray<float4> DisplacementMap : register(t0);
Texture2DArray<float4> DerivativeMap : register(t1);
StructuredBuffer<CombineParameter> parameters : register(t2);

[numthreads(16, 16, CASCADE_COUNT)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float Dxz = DisplacementMap[DTid].w;
	float2 DxxDzz = DerivativeMap[DTid].zw;
	
	
	float jxx = 1.0 + DxxDzz.x;
	float jzz = 1.0 + DxxDzz.y;
	float jxz = Dxz;
	
	float jacobian = jxx * jzz - jxz * jxz;
	float jminus = 0.5 * (jxx + jzz) - 0.5 * sqrt((jxx - jzz) * (jxx - jzz) + 4.0 * jxz * jxz);
	
	float bias = 1.0;
	float2 current = float2(-jminus, -jacobian) + bias;
	float2 persistent = TurbulencenessTex[DTid].zw; // prev
	
	// persistent -= FoamDecayRate * DeltaTime;
	persistent = 0.01 * 0.003;
	persistent = max(current, persistent);
	
	TurbulencenessTex[DTid] = float4(current, persistent);
}