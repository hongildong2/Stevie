#include "Common.hlsli"
#include "GPURandom.hlsli"
#include "OceanFunctions.hlsli"

// https://github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/InitialSpectrum.compute


// These contains all TARGET_COUNT spectrum waves with z-indexed texture
RWTexture2DArray<float2> initialSpectrumTex : register(u0);

// wave vector x, 1 / magnitude, wave vector z, frequency
RWTexture2DArray<float4> wavesDataTex : register(u1);

cbuffer WaveConstant : register(b0)
{
	float4 lengthScales;
	float cutoffHigh;
	float cutoffLow;
	float g;
	float depth;
}

static uint size = SIZE;

// These contains all TARGET_COUNT spectrum wave cascades with z-indexed
StructuredBuffer<SpectrumParameters> waveSpectrums; // 2 x TARGET_COUNT


[numthreads(16, 16, TARGET_COUNT)] // calc simultaneously all three wave cascades
void main( uint3 DTid : SV_DispatchThreadID )
{
	const float waveLengthScales[TARGET_COUNT] = { lengthScales.x, lengthScales.y, lengthScales.z, lengthScales.w };
	const float selectedWaveLengthScale = waveLengthScales[DTid.z];
	float deltaK = 2 * GPU_PI / selectedWaveLengthScale;
	int nx = DTid.x - size / 2;
	int nz = DTid.y - size / 2;
	float2 k = float2(nx, nz) * deltaK; // wave vector k, 2pi/L * wave number
	
	float kLength = length(k);
	
	// clamp
	if (kLength <= cutoffHigh && kLength >= cutoffLow)
	{
		float kAngle = atan2(k.y, k.x);
		float w = Frequency(kLength, g, depth);
		
		float dwdk = FrequencyDerivative(kLength, g, depth);
		
		
		const uint waveCascadeIndex = DTid.z;
		
		// just random sampling of JONSWAP spectrum of ocean wave
		float waveSpectrum = JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex]) * DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex]) * ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex]);
		
		if (waveSpectrums[waveCascadeIndex + 1].scale > 0) // ??
		{
			waveSpectrum += JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex + 1])
			* DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex + 1])
			* ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex + 1]);
		}
				
		float2 randomNoise = float2(NormalRandom(DTid), NormalRandom(uint3(DTid.xy, waveCascadeIndex + TARGET_COUNT)));
		
		// {wave vector x, 1 / magnitude, wave vector z, frequency}
		wavesDataTex[DTid] = float4(k.x, 1 / kLength, k.y, w);
		
		// ?? 어디서 온 식??
		initialSpectrumTex[DTid] = randomNoise * sqrt(2 * waveSpectrum * abs(dwdk) / kLength * deltaK * deltaK);
	}
	else
	{
		initialSpectrumTex[DTid] = float2(0.0, 0.0);
		wavesDataTex[DTid] = float4(k.x, 1, k.y, 0);
	}
}