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


// (m)
float GetScaler(float simulationScale, float cascadeScale)
{
	return simulationScale / cascadeScale;
}

float2 GetScaledUV(float uv, float simulationScaleInMeter, float cascadeScaleInMeter)
{
	return (simulationScaleInMeter / cascadeScaleInMeter) * uv;
}

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
		
		float2 scaledOffset = offset * uvScaler; // 얘도 텍스쳐좌표계로..
		float2 scaledUV = float2(uvScaler * UV.x, uvScaler * UV.y);
		float valueScaler = simulationScale / parameters[cascade].L;
		
		
		// TODO : 밸류에 Scaler 적용하는게 올바른가? 보기로는 스케일링 안하는게 더 나음
		// float3 sampledDisplacement = valueScaler * DisplacementMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0).xyz;
		float3 sampledDisplacement = DisplacementMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0).xyz;
		
		displacement += factor * sampledDisplacement;
	}

	return displacement; // 얘 단위는 m겠지?
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
		
		// sampledDerivative.z *= valueScaler; // TODO 왜 z부분만 스케일링 영향이 있는지 연구해보기!!, 스케일링ㅇ을해야하나?
		
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
	// => 멀티샘플링
	HeightMap[DTid.xy] = displacement.y;
	NormalMap[DTid.xy] = float4(GetNormalFromDerivative(XZ_INDEX), 0.0); // only xyz is meaningful, UAV does not support float3
	
	
	// 서로다른 스케일의 데이터를 어떻게 하나로 합칠것인가? 텍스쳐의 좌표가 서로 동등하다고 볼 수 있나?
	// 내가 렌더링하고싶은 사각형의 스케일을 정해야한다. 512m x 512m 크기의 바다를 사각형 Quad로 표현한다고 하면
	// 512x512 높이맵에서 1x1 그리드의 크기는 1mx1m 일것이다
	// 각 cascade Displacement 그리드의 크기는 각각 L / 512 :: 2m, 0.5m, 0.03m, 0.005m 이런식으로 될것이다
	// 서로 좌표계를 Normalize해서 동일하게해야한다! => 텍스쳐좌표계로 통일해서 샘플러를 쓰자
	// 스레드 인덱스를 Quad의 크기로 나눈다 -> HeightMap에 입력할 텍스쳐 좌표 얻엇다.
	// 배수 파라미터  scaler = 1 / (Cascade.L /  OceanQuadSize) 를 텍스쳐 좌표에 곱해서 바둑판식으로 가져오도록
	// scaler = min(1, scaler); 굳이 필요한지는 모르겠지만 텍스쳐를 다 쓰고싶어서.
	// 이러고 샘플러를, 바둑판형식으로 샘플링하는 WRAP 또는 MIRROR 텍스쳐 샘플러사용
}


