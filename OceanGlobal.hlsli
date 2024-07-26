#ifndef __OCEAN_GLOBAL__
#define __OCEAN_GLOBAL__

#define SIZE 512 // 512 by 512 DFT
#define LOG_SIZE 9 // log2  512
#define CASCADE_COUNT 4 // total 4 wave cascades


/* JONSWAP Spectrum Functions*/
struct SpectrumParameters
{
	float L;
	float scale;
	float angle;
	float spreadBlend;
	float swell;
	float alpha;
	float peakOmega;
	float gamma;
	float shortWavesFade;
	float cutoffLow; // JONSWAP SPECTRUM cutoffs, different for each cascade
	float cutoffHigh;
	float dummy;
};

float Frequency(float k, float g, float depth)
{
	return sqrt(g * k * tanh(min(k * depth, 20.0)));
}

float FrequencyDerivative(float k, float g, float depth)
{
	float th = tanh(min(k * depth, 20));
	float ch = cosh(k * depth);
	return g * (depth * k / ch / ch + th) / Frequency(k, g, depth) / 2;
}

float NormalisationFactor(float s)
{
	float s2 = s * s;
	float s3 = s2 * s;
	float s4 = s3 * s;
	if (s < 5)
		return -0.000564 * s4 + 0.00776 * s3 - 0.044 * s2 + 0.192 * s + 0.163;
	else
		return -4.80e-08 * s4 + 1.07e-05 * s3 - 9.53e-04 * s2 + 5.90e-02 * s + 3.93e-01;
}

float Cosine2s(float theta, float s)
{
	return NormalisationFactor(s) * pow(abs(cos(0.5 * theta)), 2 * s);
}

float SpreadPower(float omega, float peakOmega)
{
	if (omega > peakOmega)
	{
		return 9.77 * pow(abs(omega / peakOmega), -2.5);
	}
	else
	{
		return 6.97 * pow(abs(omega / peakOmega), 5);
	}
}

float DirectionSpectrum(float theta, float omega, SpectrumParameters param)
{
	float s = SpreadPower(omega, param.peakOmega)
		+ 16 * tanh(min(omega / param.peakOmega, 20)) * param.swell * param.swell;
	return lerp(2 / 3.1415 * cos(theta) * cos(theta), Cosine2s(theta - param.angle, s), param.spreadBlend);
}

float ShortWavesFade(float kLength, SpectrumParameters param)
{
	return exp(-param.shortWavesFade * param.shortWavesFade * kLength * kLength);
}

float TMACorrection(float omega, float g, float depth)
{
	float omegaH = omega * sqrt(depth / g);
	
	if (omegaH <= 1)
	{
		return 0.5 * omegaH * omegaH;
	}
	else if (omegaH < 2)
	{
		return 1.0 - 0.5 * (2.0 - omegaH) * (2.0 - omegaH);
	}
	else
	{
		return 1;
	}
}

// https:wikiwaves.org/Ocean-Wave_Spectra
float JONSWAP(float omega, float g, float depth, SpectrumParameters param)
{
	float sigma = omega <= param.peakOmega ? 0.07 : 0.09;

	float r = exp(-(omega - param.peakOmega) * (omega - param.peakOmega)
		/ 2 / sigma / sigma / param.peakOmega / param.peakOmega);
	
	float oneOverOmega = 1 / omega;
	float peakOmegaOverOmega = param.peakOmega / omega;
	return param.scale * TMACorrection(omega, g, depth) * param.alpha * g * g
		* oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega
		* exp(-1.25 * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega)
		* pow(abs(param.gamma), r);
}

/* Wave Cascades */

struct CombineParameter
{
	float L;
	float weight;
	float shoreModulation;
	float dummy;
};

float2 GetScaledUV(float2 uv, float simulationScaleInMeter, float cascadeScaleInMeter)
{
	return (simulationScaleInMeter / cascadeScaleInMeter) * uv;
}


// can sample displacement map and normal map both
float4 SampleDisplacementModel(Texture2DArray<float4> displacementMaps, StructuredBuffer<CombineParameter> parameters, SamplerState ss, uint cascadesCount, float2 uv, float simulationScaleInMeter)
{
	float4 result = 0;
	
	[unroll(cascadesCount)] // ???
	for (uint cascadeIndex = 0; cascadeIndex < cascadesCount; ++cascadeIndex)
	{
	
		float scaler = simulationScaleInMeter / parameters[cascadeIndex].L;
		float configFactor = parameters[cascadeIndex].weight * parameters[cascadeIndex].shoreModulation;
		float2 scaledUV = GetScaledUV(uv, simulationScaleInMeter, parameters[cascadeIndex].L);
		
		float4 sampled = scaler * displacementMaps.SampleLevel(ss, float3(scaledUV, cascadeIndex), 0);
	
	
		// TODO : 샘플한 밸류에 시뮬레이션 크기 고려 스케일링 적용할까말까
		result += configFactor * sampled;
	}
	
	return result;
}

float3 MultiSampleDisplacementModel(Texture2DArray<float4> displacementMaps, StructuredBuffer<CombineParameter> parameters, SamplerState ss, uint cascadesCount, float2 uv, float simulationScaleInMeter)
{
	float2 UV = uv;
	
	float3 displacement = SampleDisplacementModel(displacementMaps, parameters, ss, CASCADE_COUNT, UV, simulationScaleInMeter).xyz;
	
	
	
	for (uint i = 0; i < 4; ++i)
	{
		float2 offsetInTexCoord = float2(displacement.x / simulationScaleInMeter, displacement.z / simulationScaleInMeter);
		UV -= offsetInTexCoord;
		displacement = SampleDisplacementModel(displacementMaps, parameters, ss, CASCADE_COUNT, UV, simulationScaleInMeter).xyz;
	}

	return displacement;
}


float3 SampleNormalModel(Texture2DArray<float4> derivativeMaps, StructuredBuffer<CombineParameter> parameters, SamplerState ss, uint cascadesCount, float2 uv, float simulationScaleInMeter)
{
	float4 derivative = 0;
	for (uint cascadeIndex = 0; cascadeIndex < cascadesCount; ++cascadeIndex)
	{
		// float configFactor = parameters[cascadeIndex].weight * parameters[cascadeIndex].shoreModulation;
		float2 scaledUV = GetScaledUV(uv, simulationScaleInMeter, parameters[cascadeIndex].L);
		
		float4 sampled = derivativeMaps.SampleLevel(ss, float3(scaledUV, cascadeIndex), 0);
		
		sampled.z *= simulationScaleInMeter / parameters[cascadeIndex].L;
		
		derivative += sampled;
	}
	
	float2 slope = float2(derivative.x / max(0.001, 1 + derivative.z), derivative.y / max(0.001, 1 + derivative.w));
	
	return normalize(float3(-slope.x, 1, -slope.y));
}
#endif /* __OCEAN_GLOBAL__ */