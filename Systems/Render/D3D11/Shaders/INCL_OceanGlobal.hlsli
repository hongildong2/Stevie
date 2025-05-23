#ifndef __OCEAN_GLOBAL__
#define __OCEAN_GLOBAL__

#define SIZE 1024 // 512 by 512 DFT
#define FFT_SIZE 1024 // if SIZE < 1024, SIZE = FFT_SIZE
#define LOG_SIZE 10 // log2  1024
#define CASCADE_COUNT 4 // total 4 wave cascades
#define SAMPLING_COUNT 9
#define SIMULATION_SIZE_IN_METER 2048.0

#define DELTA 0.001


static const float2 SAMPLING_KERNEL[SAMPLING_COUNT] =
{
	float2(-1.0 * DELTA, -1.0 * DELTA),
	float2(0.0 * DELTA, -1.0 * DELTA),
	float2(1.0 * DELTA, -1.0 * DELTA),
			 
	float2(-1.0 * DELTA, 0.0 * DELTA),
	float2(0.0 * DELTA, 0.0 * DELTA),
	float2(1.0 * DELTA, 0.0 * DELTA),
			 
	float2(-1.0 * DELTA, 1.0 * DELTA),
	float2(0.0 * DELTA, 1.0 * DELTA),
	float2(1.0 * DELTA, 1.0 * DELTA),
};

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

struct OceanSamplingInput
{
	Texture2DArray<float4> tex;
	StructuredBuffer<CombineParameter> parameters;
	SamplerState ss;
	uint cascadesCount;
	float2 uv;
	float simulationScaleInMeter;
};

float2 GetScaledUV(float2 uv, float simulationScaleInMeter, float cascadeScaleInMeter)
{
	return max(1.0, (simulationScaleInMeter / cascadeScaleInMeter)) * uv;
}

float4 SampleOceanTexture(OceanSamplingInput input)
{
	float4 result = 0;
	
	// [unroll(CASCADE_COUNT)]
	for (uint cascadeIndex = 0; cascadeIndex < input.cascadesCount; ++cascadeIndex)
	{
		// 밸류 스케일링은 안하는게 맞는듯
		// float scaler = simulationScaleInMeter / parameters[cascadeIndex].L;
		
		float configFactor = input.parameters[cascadeIndex].weight * input.parameters[cascadeIndex].shoreModulation;
		float2 scaledUV = GetScaledUV(input.uv, input.simulationScaleInMeter, input.parameters[cascadeIndex].L);
				
		float4 sampled = input.tex.SampleLevel(input.ss, float3(scaledUV, cascadeIndex), 0);

		result += configFactor * sampled;
	}
	
	return result;
}

float3 MultiSampleDisplacementModel(OceanSamplingInput input)
{
	float2 UV = input.uv;
	
	float3 displacement = 0;
	[unroll(SAMPLING_COUNT)]
	for (uint i = 0; i < SAMPLING_COUNT; ++i)
	{
		float2 multiUV = UV + SAMPLING_KERNEL[i];
		displacement += SampleOceanTexture(input).xyz;
	}
	
	return displacement / float(SAMPLING_COUNT);
}

float3 SampleNormalModel(OceanSamplingInput input)
{
	float4 derivative = SampleOceanTexture(input);
	float2 slope = float2(derivative.x / max(0.001, 1.0 + derivative.z), derivative.y / max(0.001, 1.0 + derivative.w));
	
	return normalize(float3(-slope.x, 1, -slope.y));
}

struct FoamOutput
{
	float coverage; // lerp(color, albedo, coverage)
	float3 albedo;
};

struct FoamParameter
{
	float waveSharpness;
	float foamPersistency;
	float foamDensity;
	float foamTrailness;
	float foamCoverage;
	float foamValueBias;
};

struct FoamInput
{
	OceanSamplingInput oceanSampling;
	FoamParameter foamParam;
	float2 worldUV;
	float viewDist;
};


float FoamCoverage(float4 turbulence, float2 worldUV, FoamParameter param)
{
	float foamCurrentVal = lerp(turbulence.y, turbulence.x, param.waveSharpness);
	float foamPersistentValue = (turbulence.z + turbulence.w) * 0.5;

	
	foamCurrentVal = lerp(foamCurrentVal, foamPersistentValue, param.foamPersistency);
	foamCurrentVal -= 1;
	foamPersistentValue -= 1;
	
	float foamValue = max((foamPersistentValue + param.foamTrailness * (1 - param.foamValueBias)), (foamCurrentVal + param.foamCoverage * (1 - param.foamValueBias)));
	float surfaceFoam = saturate(foamValue * param.foamDensity);

	return surfaceFoam;
}


void ScreenSpaceContactFoam(in FoamOutput foamRendered, Texture2D <float>depthMap, SamplerState ss, float4 positionProjection, matrix cameraInvProj)
{
	float2 positionScreenSpace = positionProjection.xy;
	float2 texcoord = float2(positionScreenSpace.x, -positionScreenSpace.y);
	texcoord += 1.0;
	texcoord *= 0.5;
	
	float objDepth = depthMap.Sample(ss, positionScreenSpace);

	float4 pointView = mul(float4(0, 0, positionProjection.z, 1), cameraInvProj);
	pointView.z /= pointView.w;
	float viewDepth = pointView.z;
	
	float depthDiff = objDepth - viewDepth;
	
	const float CONTACT_FOAM_RANGE = 0.2;
	float contactFoam = saturate(10 * (CONTACT_FOAM_RANGE - depthDiff));
	
	foamRendered.coverage = saturate(foamRendered.coverage + contactFoam);
}

float3 LitFoamColor(FoamOutput foamData, TextureCube irradianceMap, SamplerState ss, float3 N, float NdotL, float3 lightColor, float lightAttenuation)
{
	const float3 FOAM_TINT_RGB = 0;
	float ndotl = (0.2 + 0.8 * NdotL) * lightAttenuation;
	float3 envDiffuse = irradianceMap.Sample(ss, N);
	return foamData.albedo * (ndotl * lightColor + envDiffuse);
}


FoamOutput GetFoamOutput(Texture2D<float> foamTexture, SamplerState ss, FoamInput input)
{
	FoamOutput res;
	float4 turbulence = SampleOceanTexture(input.oceanSampling);
	
	res.coverage = FoamCoverage(turbulence, input.worldUV, input.foamParam);
	
	// 가까이서보면 Foam Coverage를 극적으로 줄인다.
	res.coverage *= 1 - saturate((1.5 + input.viewDist * 0.5 - 2000.0) * 0.0005);
	
	
	res.albedo = foamTexture.Sample(ss, input.worldUV * 48.0).rrr;

	
	return res;
}

/* Foam Simulation */

#endif /* __OCEAN_GLOBAL__ */