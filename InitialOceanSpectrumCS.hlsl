#include "Common.hlsli"
#include "GPURandom.hlsli"
#include "OceanFunctions.hlsli"

// https://github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/InitialSpectrum.compute


// These contains all TARGET_COUNT spectrum waves with z-indexed texture
RWTexture2DArray<float2> initialSpectrumTex : register(u0);

// wave vector x, 1 / magnitude, wave vector z, frequency
RWTexture2DArray<float4> wavesDataTex : register(u1);

// even num is local wave, odd num is wind wave
// These contains all TARGET_COUNT spectrum wave cascades with z-indexed
StructuredBuffer<SpectrumParameters> waveSpectrums : register(t0); // 2 x TARGET_COUNT


cbuffer WaveConstant : register(b0)
{
	float cutoffHigh;
	float cutoffLow;
	float g;
	float depth;
}

static uint size = SIZE;



[numthreads(16, 16, TARGET_COUNT)] // calc simultaneously all three wave cascades
void main( uint3 DTid : SV_DispatchThreadID )
{
	const uint waveCascadeIndex = DTid.z;
	float deltaK = 2 * GPU_PI / waveSpectrums[waveCascadeIndex].lengthScale;
	uint nx = DTid.x - size / 2;
	uint nz = DTid.y - size / 2;
	float2 k = float2(nx, nz) * deltaK; // wave vector k, 2pi/L * wave number
	
	float kLength = length(k);
	
	// clamp
	if (kLength <= cutoffHigh && kLength >= cutoffLow)
	{
		float2 randomNoise = float2(NormalRandom(DTid), NormalRandom(uint3(DTid.xy, waveCascadeIndex + TARGET_COUNT)));
		float kAngle = atan2(k.y, k.x);
		float w = Frequency(kLength, g, depth);
		
		float dwdk = FrequencyDerivative(kLength, g, depth);
		
		// just random sampling of JONSWAP spectrum of ocean wave
		// float waveSpectrum = JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex]) * DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex]) * ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex]);
		
		// float waveSpectrum = JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex]) * DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex]);
		
		// TODO : 텍스쳐에 찍어보면서 값 확인, 현재 JONSWAP 함수 출력값 자체가 0이거나 NaN이다.
		float waveSpectrum = JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex]);
		
		if (waveSpectrums[waveCascadeIndex + 1].scale > 0) // wind wave
		{
			waveSpectrum += JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex + 1])
			* DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex + 1])
			* ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex + 1]);
		}
				

		
		// {wave vector x, 1 / magnitude, wave vector z, frequency}
		wavesDataTex[DTid] = float4(k.x, 1 / kLength, k.y, w);
		
		// ?? 어디서 온 식??
		float res = sqrt(2 * waveSpectrum * abs(dwdk) / kLength * deltaK * deltaK);
		
		initialSpectrumTex[DTid] = float2(randomNoise.x * res, randomNoise.y * res);
	}
	else
	{
		initialSpectrumTex[DTid] = float2(0.0, 0.0);
		wavesDataTex[DTid] = float4(k.x, 1, k.y, 0);
	}
}