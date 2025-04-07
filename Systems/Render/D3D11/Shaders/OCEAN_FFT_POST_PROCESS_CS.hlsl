// https:github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/FastFourierTransform.compute
// https:github.com/gasgiant/Ocean-URP/blob/main/Assets/OceanSystem/Shaders/Resources/ComputeShaders/FFT.compute

#include "INCL_OceanGlobal.hlsli"

// z indexed wave cascades
RWTexture2DArray<float4> FTResultTex : register(u0);

cbuffer FFTInfo : register(b0)
{
	uint targetCount;
	bool direction;
	bool inverse;
	bool scale;
	bool permute;
}


float4 DoPostProcess(float4 input, uint2 id)
{
	if (scale)
		input /= SIZE * SIZE;
	if (permute)
		input *= 1.0 - 2.0 * ((id.x + id.y) % 2);
	return input;
}

// wave post process
[numthreads(16, 16, CASCADE_COUNT)]
void main(uint3 id : SV_DispatchThreadID)
{
	FTResultTex[id] = DoPostProcess(FTResultTex[id], id.xy);
}