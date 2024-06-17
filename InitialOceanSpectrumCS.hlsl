#include "Common.hlsli"
#include "Random.hlsli"
#include "OceanFunctions.hlsli"

// https://github.com/gasgiant/FFT-Ocean/blob/main/Assets/ComputeShaders/InitialSpectrum.compute


// These contains all 3 spectrum waves with z-indexed
RWTexture2DArray<float4> initialSpectrumTex; 
RWTexture2DArray<float4> wavesDataTex;

Texture2D<float2> noiseTex;

cbuffer WaveConstant : register(b0)
{
	float3 lengthScale;
	float cutoffHigh;
	float cutoffLow;
	float g;
	float depth;
	float dummy;
}

static uint size = SIZE;

// These contains all TARGET_COUNT spectrum wave cascades with z-indexed
StructuredBuffer<SpectrumParameters> waveSpectrums; // 2 x TARGET_COUNT
const float waveLengthScales[TARGET_COUNT] = { lengthScale.x, lengthScale.y, lengthScale.z };





[numthreads(16, 16, TARGET_COUNT)] // calc simultaneously all three wave cascades
void main( uint3 DTid : SV_DispatchThreadID )
{
	const float selectedWaveLengthScale = waveLengthScales[DTid.z];
	float deltaK = 2 * PI / selectedWaveLengthScale;
	int nx = DTid.x - size / 2;
	int nz = DTid.y - size / 2;
	float2 k = float2(nx, nz) * deltaK; // wave vector k, 2pi/L * wave number
	
	float kLength = length(k);
	
	// overflow
	if (kLength <= cutoffHigh && kLength >= cutoffLow)
	{
		float kAngle = atan2(k.y, k.x);
		float w = Frequency(kLength, g, depth);
		
		float dwdk = FrequencyDerivative(kLength, g, depth);
		
		
		const uint waveCascadeIndex = DTid.z;
		
		// just random smapling of spectrum of ocean wave as signal
		float waveSpectrum = JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex]) * DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex]) * ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex]);
		
		if (waveSpectrums[waveCascadeIndex + 1].scale > 0)
		{
			waveSpectrum += JONSWAP(w, g, depth, waveSpectrums[waveCascadeIndex + 1])
			* DirectionSpectrum(kAngle, w, waveSpectrums[waveCascadeIndex + 1])
			* ShortWavesFade(kLength, waveSpectrums[waveCascadeIndex + 1]);
		}
				
		float2 randomNoise = float2(NormalRandom(DTid), NormalRandom(uint3(DTid.xy, waveCascadeIndex + TARGET_COUNT)));
		wavesDataTex[DTid] = float4(k.x, 1 / kLength, k.y, w);
		initialSpectrumTex[DTid] = randomNoise * sqrt(2 * waveSpectrum * abs(dwdk) / kLength * deltaK * deltaK);
	}
	else
	{
		initialSpectrumTex[DTid] = 0;
		wavesDataTex[DTid] = float4(k.x, 1, k.y, 0);
	}
}