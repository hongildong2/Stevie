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
    // ��ó�� Ž��
	int numSteps = 128 / 4;
	float stepSize = 2.0 / float(numSteps);
	float absorptionCoeff = 50.0;

	float alpha = 1.0; // visibility 1.0���� ����

    [loop] // [unroll] ��� �� ���̴� ������ �ʹ� ����
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
    
    // alpha�� 0�� ����� ���� �������κ��� ���� �� �� ����
	return alpha;
}

[numthreads(16, 16, 4)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width, height, depth;
	lightingTex.GetDimensions(width, height, depth);
    
	float3 uvw = DTid / float3(width, height, depth);

    // uvw�� [0, 1]x[0,1]x[0,1]
    // �� ��ǥ��� [-1,1]x[-1,1]x[-1,1]
	lightingTex[DTid] = LightRay(densityTex, linearClamp, (uvw - 0.5) * 2.0, float3(0, 1, 0));
}