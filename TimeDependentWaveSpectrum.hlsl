#include "OceanFunctions.hlsli"

// https:github.com/gasgiant/Ocean-URP/blob/main/Assets/OceanSystem/Shaders/Resources/ComputeShaders/TimeDependentSpectrum.compute
RWTexture2DArray<float4> Result; // for each cascade

Texture2DArray<float4> initialSpectrums;
Texture2DArray<float4> wavesData;

cbuffer Params : register(b0)
{
	float Time;
	float3 dummy;
};

float2 ComplexMult(float2 a, float2 b)
{
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

void CalculateForCascade(uint3 id)
{
	float4 wave = wavesData[id]; // wave vector x, 1 / magnitude, wave vector z, frequency
	
	float phase = wave.w * Time;
	float2 exponent = float2(cos(phase), sin(phase));
	float4 h0 = initialSpectrums[id];
	float2 h = ComplexMult(h0.xy, exponent)
		+ ComplexMult(h0.zw, float2(exponent.x, -exponent.y));
	float2 ih = float2(-h.y, h.x);
	
	float oneOverKLength = 1 / max(0.001, length(wave.xz));
	
	float lambda = wave.y;
	float2 displacementX = lambda * ih * wave.x * oneOverKLength;
	float2 displacementY = h;
	float2 displacementZ = lambda * ih * wave.z * oneOverKLength;
	
	float2 displacementX_dx = -lambda * h * wave.x * wave.x * oneOverKLength;
	float2 displacementY_dx = ih * wave.x;
	float2 displacementZ_dx = -lambda * h * wave.x * wave.z * oneOverKLength;
		 
	float2 displacementY_dz = ih * wave.z;
	float2 displacementZ_dz = -lambda * h * wave.z * wave.z * oneOverKLength;
	
	// h tilde, spectrum
	Result[uint3(id.xy, id.z * 2)] = float4(float2(displacementX.x - displacementY.y, displacementX.y + displacementY.x),
							  float2(displacementZ.x - displacementZ_dx.y, displacementZ.y + displacementZ_dx.x));
	
	// derivative of h tilde. for Displacement map and Jacobian
	Result[uint3(id.xy, id.z * 2 + 1)] = float4(float2(displacementY_dx.x - displacementY_dz.y, displacementY_dx.y + displacementY_dz.x),
							     float2(displacementX_dx.x - displacementZ_dz.y, displacementX_dx.y + displacementZ_dz.x));
}

[numthreads(16, 16, TARGET_COUNT)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	CalculateForCascade(DTid);	
}