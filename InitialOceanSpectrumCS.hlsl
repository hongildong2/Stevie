#include "GPURandom.hlsli"
#include "OceanGlobal.hlsli"

// https://github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/InitialSpectrum.compute


// These contains all TARGET_COUNT spectrum waves with z-indexed texture
RWTexture2DArray<float2> initialSpectrumTex : register(u0);

// wave vector x, 1 / magnitude, wave vector z, frequency
RWTexture2DArray<float4> wavesDataTex : register(u1);

// even num is local wave, odd num is wind wave
// These contains all TARGET_COUNT spectrum wave cascades with z-indexed
StructuredBuffer<SpectrumParameters> LocalWaveSpectrumParameters : register(t0);
StructuredBuffer<SpectrumParameters> SwellWaveSpectrumParameters : register(t1);

cbuffer WaveConstant : register(b0)
{
	float g;
	float depth;
	float2 dummy;
}


[numthreads(16, 16, CASCADE_COUNT)] // calc simultaneously all three wave cascades
void main(uint3 DTid : SV_DispatchThreadID)
{
	const uint waveCascadeIndex = DTid.z;
	float deltaK = 2.0 * GPU_PI / LocalWaveSpectrumParameters[waveCascadeIndex].L;
	int nx = DTid.x - float(SIZE) / 2.0;
	int nz = DTid.y - float(SIZE) / 2.0;
	float2 k = float2(nx * deltaK, nz * deltaK); // wave vector k, 2pi/L * wave number
	
	float kLength = length(k);
	
	// clamp
	if (kLength <= LocalWaveSpectrumParameters[waveCascadeIndex].cutoffHigh && kLength >= LocalWaveSpectrumParameters[waveCascadeIndex].cutoffLow)
	{
		float2 randomNoise = float2(NormalRandom(DTid), NormalRandom(uint3(DTid.xy, waveCascadeIndex + CASCADE_COUNT)));
		float kAngle = atan2(k.y, k.x);
		float w = Frequency(kLength, g, depth);
		
		float dwdk = FrequencyDerivative(kLength, g, depth);
		
		// just random sampling of JONSWAP spectrum of ocean wave
		float waveSpectrum = JONSWAP(w, g, depth, LocalWaveSpectrumParameters[waveCascadeIndex])
			* DirectionSpectrum(kAngle, w, LocalWaveSpectrumParameters[waveCascadeIndex])
			* ShortWavesFade(kLength, LocalWaveSpectrumParameters[waveCascadeIndex]);
	
		if (SwellWaveSpectrumParameters[waveCascadeIndex].scale > 0) // swellwave
		{
			waveSpectrum += JONSWAP(w, g, depth, SwellWaveSpectrumParameters[waveCascadeIndex])
			* DirectionSpectrum(kAngle, w, SwellWaveSpectrumParameters[waveCascadeIndex])
			* ShortWavesFade(kLength, SwellWaveSpectrumParameters[waveCascadeIndex]);
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
		wavesDataTex[DTid] = float4(k.x, 1, k.y, 0.0);
	}
}