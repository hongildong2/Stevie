#include "OceanFunctions.hlsli"

RWTexture2D<float> HeightMap : register(u0);
RWTexture2D<float3> NormalMap : register(u1);

SamplerState wrapSampler : register(s0);

Texture2DArray<float4> DisplacementMap : register(t0);
Texture2DArray<float4> DerivativeMap : register(t1);


struct CombineParameter
{
	float L;
	float weight;
	float shoreModulation;
};

StructuredBuffer<CombineParameter> parameters : register(t2);

const float QUAD_SIZE = 512.0;
const uint N = 512; // fourier grid

float3 SampleDisplacement(uint2 xzIndex, float2 offset)
{
	const float2 UV = float2(xzIndex.x / N, xzIndex.y / N);

	float3 displacement = 0;

	[unroll(TARGET_COUNT)]
	for (uint cascade = 0; cascade < TARGET_COUNT; ++cascade)
	{
		float factor = parameters[cascade].weight * parameters[cascade].shoreModulation;
		float uvScaler = min(1.0, QUAD_SIZE / parameters[cascade].L);
		
		float2 scaledOffset = offset * uvScaler; // 얘도 텍스쳐좌표계로..
		float2 scaledUV = float2(uvScaler * UV.x, uvScaler * UV.y) - scaledOffset;
		
		displacement += factor * DisplacementMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0).xyz;
	}

	return displacement; // 얘 단위는 m겠지?
}

float3 GetNormalFromDerivative(uint2 xzIndex)
{
	const float2 UV = float2(xzIndex.x / N, xzIndex.y / N);

	float4 derivative = 0.0;
	
	[unroll(TARGET_COUNT)]
	for (uint cascade = 0; cascade < TARGET_COUNT; ++cascade)
	{
		float uvScaler = min(1.0, QUAD_SIZE / parameters[cascade].L);
		float2 scaledUV = float2(uvScaler * UV.x, uvScaler * UV.y);
		
		derivative += parameters[cascade].weight * DerivativeMap.SampleLevel(wrapSampler, float3(scaledUV, cascade), 0.0);
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
	NormalMap[DTid.xy] = GetNormalFromDerivative(XZ_INDEX);
	
	
	// TODO : 서로다른 스케일의 데이터를 어떻게 하나로 합칠것인가? 텍스쳐의 좌표가 서로 동등하다고 볼 수 있나?
	// 내가 렌더링하고싶은 사각형의 스케일을 정해야한다. 512m x 512m 크기의 바다를 사각형 Quad로 표현한다고 하면
	// 512x512 높이맵에서 1x1 그리드의 크기는 1mx1m 일것이다
	// 각 cascade Displacement 그리드의 크기는 각각 L / 512 :: 2m, 0.5m, 0.03m, 0.005m 이런식으로 될것이다
	// 서로 좌표계를 Normalize해서 동일하게해야한다! => 텍스쳐좌표계로 통일해서 샘플러를 쓰자
	// 스레드 인덱스를 Quad의 크기로 나눈다 -> HeightMap에 입력할 텍스쳐 좌표 얻엇다.
	// 배수 파라미터  scaler = 1 / (Cascade.L /  OceanQuadSize) 를 텍스쳐 좌표에 곱해서 바둑판식으로 가져오도록
	// scaler = min(1, scaler); 굳이 필요한지는 모르겠지만 텍스쳐를 다 쓰고싶어서.
	// 이러고 샘플러를, 바둑판형식으로 샘플링하는 WRAP 또는 MIRROR 텍스쳐 샘플러사용
}


