#include "INCL_GPURandom.hlsli"

SamplerState linearClamp : register(s0);

Texture3D<float> densityTex : register(t0);
RWTexture3D<float> lightingTex : register(u0);

// https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-2.html
float BeerLambert(float absorptionCoefficient, float distanceTraveled)
{
	return exp(-absorptionCoefficient * distanceTraveled);
}

float3 ModelToUVW(float3 posModel)
{
	return (posModel.xyz + 1.0) * 0.5;
}

float LightRay(Texture3D<float> densityTexture, SamplerState ss, float3 posModel, float3 lightDir)
{
    // 근처만 탐색
	int numSteps = 128 / 4;
	float stepSize = 2.0 / float(numSteps);
	float absorptionCoeff = 50.0;

	float alpha = 1.0; // visibility 1.0으로 시작

    [loop] // [unroll] 사용 시 쉐이더 생성이 너무 느림
	for (int i = 0; i < numSteps; i++)
	{
		float prevAlpha = alpha;
		float density = densityTexture.SampleLevel(ss, ModelToUVW(posModel), 0); // linearClamp
        
		if (density > 1e-3)
			alpha *= BeerLambert(absorptionCoeff * density, stepSize);

		posModel += lightDir * stepSize;

		if (abs(posModel.x) > 1 || abs(posModel.y) > 1 || abs(posModel.z) > 1)
			break;
        
		if (alpha < 1e-3)
			break;
	}
    
    // alpha가 0에 가까울 수록 조명으로부터 빛을 잘 못 받음
	return alpha;
}

[numthreads(16, 16, 4)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width, height, depth;
	lightingTex.GetDimensions(width, height, depth);
    
	float3 uvw = DTid / float3(width, height, depth);

    // uvw는 [0, 1]x[0,1]x[0,1]
    // 모델 좌표계는 [-1,1]x[-1,1]x[-1,1]
	lightingTex[DTid] = LightRay(densityTex, linearClamp, (uvw - 0.5) * 2.0, float3(0, 1, 0));
}