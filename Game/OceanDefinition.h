#pragma once
#include "pch.h"
#include <array>

namespace ocean
{
	constexpr unsigned int CASCADE_COUNT = 4; // total 4 different wave cascade
	constexpr unsigned int N = 1024; // fourier grid size, M = N

	constexpr unsigned int GROUP_X = N / 16;
	constexpr unsigned int GROUP_Y = N / 16;

	constexpr float DEFAULT_G = 9.8f;
	constexpr float DEFAULT_DEPTH = 400.f;

	constexpr float WORLD_SCALER = 75.f;
	constexpr float SIMULATION_SIZE_IN_METER = 2048.f;

	struct OceanConfigurationConstant
	{
		float g;
		float depth;
		float dummy[2];
	};
	static_assert(sizeof(OceanConfigurationConstant) % 16 == 0, "CB 16byte alignment");

	constexpr OceanConfigurationConstant OceanConfigurationInitializer =
	{
		DEFAULT_G,
		DEFAULT_DEPTH,
		{ 0.f }
	};

	struct CombineWaveConstant
	{
		float simulationScale; // ocean quad size
		BOOL bScale; // boolean for N*N scaling,
		float dummy[2];
	};
	static_assert(sizeof(CombineWaveConstant) % 16 == 0, "CB 16byte alignment");
	constexpr CombineWaveConstant CombineWaveConstantInitializer =
	{
		SIMULATION_SIZE_IN_METER,
		FALSE,
		{0.f}
	};

	struct InitialSpectrumParameter
	{
		float L; // Simulation Size, grid size dx will be L / N
		float scale; // decide how much effect this wave will have
		float angle; // wind dir, in radian
		float spreadBlend; // 0 ~ 1
		float swell; // 0.01 ~ 1
		float alpha; // jonswap factor, typically default to 0.0081
		float peakOmega; // jonswap factor, typically default to 0.5
		float gamma; // peak enhancement, default to 3.3
		float shortWavesFade;
		// JONSWAP SPECTRUM cutoffs, different for each cascade
		float cutoffLow;
		float cutoffHigh;
		float dummy;
	};
	static_assert(sizeof(InitialSpectrumParameter) % 16 == 0, "CB 16byte alignment");

	struct CombineParameter
	{
		float L;
		float weight;
		float shoreModulation;
		float dummy;
	};
	static_assert(sizeof(CombineParameter) % 16 == 0, "CB 16byte alignment");

	// (m) 
	constexpr float DEFAULT_JONSWAP_FETCH_LENGTH_LOCAL = 100.0f;
	constexpr float DEFAULT_JONSWAP_FETCH_LENGTH_SWELL = 100.0f;

	constexpr float DEFAULT_JONSWAP_WIND_DIR = -29.81f;
	constexpr float DEFAULT_JONSWAP_SPREAD_BLEND = 1.f;
	constexpr float DEFAULT_JONSWAP_SWELL = 0.198f;
	constexpr float DEFAULT_JONSWAP_ALPHA = 0.0081f; // 아래함수에서 직접 계산 0.4461
	constexpr float DEFAULT_JONSWAP_PEAK_OMEGA = 0.5f; // 아래함수에서 직접 계산 0.58
	constexpr float DEFAULT_JONSWAP_GAMMA = 3.3f;
	constexpr float DEFAULT_JONSWAP_WIND_SPEED = 5.f; // (m/s)
	constexpr float DEFAULT_JONSWAP_FADE = 0.01f;

	// https://dirsig.cis.rit.edu/docs/new/wavespectruminterface.html#:~:text=the%20JONSWAP%20spectrum%20is%20parametrized,100km)%20and%203.3%20%2C%20respectively.


	constexpr float JONSWAP_CUTOFF_MIN = 0.0001f;
	constexpr float JONSWAP_CUTOFF_MAX = 300.f; // 너무 긴 wave length를 허용하면 float 연산중 너무 작은값들이 등장해 IFFT 도중 NaN이 나와버림

	constexpr float JonswapCutoffBoundary(float L)
	{
		return 2.f * 3.14f / L * 6.f;
	}

	constexpr InitialSpectrumParameter JONSWAP_SPECTRUM_PARAMETER(float L, float scale, float windDir, float cutoffLow, float cutoffHigh)
	{
		return
		{
			L,
			scale,
			windDir,
			DEFAULT_JONSWAP_SPREAD_BLEND,
			DEFAULT_JONSWAP_SWELL,
			DEFAULT_JONSWAP_ALPHA,
			DEFAULT_JONSWAP_PEAK_OMEGA,
			DEFAULT_JONSWAP_GAMMA,
			DEFAULT_JONSWAP_FADE,
			cutoffLow,
			cutoffHigh,
			0.f
		};
	}

	//float JonswapAlpha(float g, float fetch, float windSpeed)
	//{
	//	return 0.076f * std::pow(g * fetch / windSpeed / windSpeed, -0.22f);
	//}

	//float JonswapPeakFrequency(float g, float fetch, float windSpeed)
	//{
	//	return 22 * std::pow(windSpeed * fetch / g / g, -0.33f);
	//}



	constexpr float CASCADE_1_L = 2048.f;
	constexpr float CASCADE_2_L = 256.f;
	constexpr float CASCADE_3_L = 17.f;
	constexpr float CASCADE_4_L = 3.f;

	constexpr float TEMP_DELTA_TIME = 0.03f;


	constexpr InitialSpectrumParameter LOCAL_CASCADE_1 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_1_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JONSWAP_CUTOFF_MIN, JonswapCutoffBoundary(CASCADE_2_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_2 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_2_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_2_L), JonswapCutoffBoundary(CASCADE_3_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_3 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_3_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_3_L), JonswapCutoffBoundary(CASCADE_4_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_4 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_4_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_4_L), JONSWAP_CUTOFF_MAX);

	constexpr InitialSpectrumParameter SWELL_CASCADE_1 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_1_L, 0.5f, 0.f, JONSWAP_CUTOFF_MIN, JonswapCutoffBoundary(CASCADE_2_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_2 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_2_L, 0.5f, 0.f, JonswapCutoffBoundary(CASCADE_2_L), JonswapCutoffBoundary(CASCADE_3_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_3 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_3_L, 0.5f, 0.f, JonswapCutoffBoundary(CASCADE_3_L), JonswapCutoffBoundary(CASCADE_4_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_4 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_4_L, 0.5f, 0.f, JonswapCutoffBoundary(CASCADE_4_L), JONSWAP_CUTOFF_MAX);

	constexpr std::array<CombineParameter, CASCADE_COUNT> CombineParameterInitializer =
	{ {
		{ CASCADE_1_L, 0.082f, 1.f, 0.f },
		{CASCADE_2_L, 0.073f, 1.f, 0.f},
		{CASCADE_3_L, 0.05f, 1.f, 0.f},
		{CASCADE_4_L, 0.04f, 1.f, 0.f},
	} };

	constexpr std::array<InitialSpectrumParameter, CASCADE_COUNT> LocalInitialSpectrumParameterInitializer =
	{ {
		LOCAL_CASCADE_1,
		LOCAL_CASCADE_2,
		LOCAL_CASCADE_3,
		LOCAL_CASCADE_4,
	} };
	static_assert(sizeof(LocalInitialSpectrumParameterInitializer) / sizeof(InitialSpectrumParameter) == CASCADE_COUNT, "CASCADE COUNT INCONSISTENT");

	constexpr std::array<InitialSpectrumParameter, CASCADE_COUNT> SwellInitialSpectrumParameterConstantInitializer =
	{ {
		SWELL_CASCADE_1,
		SWELL_CASCADE_2,
		SWELL_CASCADE_3,
		SWELL_CASCADE_4,
	} };
	static_assert(sizeof(SwellInitialSpectrumParameterConstantInitializer) / sizeof(InitialSpectrumParameter) == CASCADE_COUNT, "CASCADE COUNT INCONSISTENT");


	struct FFTConstant
	{
		unsigned int targetCount;
		BOOL bDirection; // bool size must be consistent with hlsl's one
		BOOL bInverse;
		BOOL bScale;
		BOOL bPermute;
		BOOL dummy[3];
	};
	static_assert(sizeof(FFTConstant) % 16 == 0, "CB 16byte alignment");
	constexpr FFTConstant FFTConstantInitializer = { ocean::CASCADE_COUNT, FALSE, TRUE, FALSE, TRUE, {FALSE,} };

	struct SpectrumConstant
	{
		float time;
		float dummy[3];
	};
	static_assert(sizeof(SpectrumConstant) % 16 == 0, "CB 16byte alignment");
	constexpr SpectrumConstant SpectrumConstantInitializer = { 0.f , {0.f} };


	// Rendering Parameters
	struct RenderingParameter
	{
		float horizonFogParameter; // 1
		float sssNormalStrength; // 0.1, SubsurfacScattering
		float sssOceanWaveReferenceHeight; // 3;
		float sssWaveHeightBias; // 1.3

		float sssSunStrength; // 10 ~
		float sssEnvironmentStrength; // 5.5
		float directLightScaler; // 50 ~
		float roughnessMultiplier; // 16 ~

		DirectX::SimpleMath::Vector3 depthScatterColor; // oceanColor * 0.7;
		float sssSpread; // 0.022

		DirectX::SimpleMath::Vector3 sssColor; // oceanColor * 0.9
		float sssFadeDistance; // 5.3

		// Wave Variance, affects fresnel and roughness
		float windSpeed; // 1.8
		float waveAlignment; // 60 ~
		float scale; // 2048 ~
		float meanFresnelWeight; // 0.033

		float specularStrength; // ~ 1
		float shadowMultiplier; // 1
		float foamWaveSharpness; // 0.9 :: Foam Parameters
		float foamPersistency; // 0.5

		float foamDensity; //0.11
		float foamCoverage; // 0.65
		float foamTrailness; // 0.5
		float foamValueBias; // 0.03 0~1

		float roughness;
		DirectX::SimpleMath::Vector3 albedo;

		float metallic;
		float IBLStrength;
		DirectX::SimpleMath::Vector2 dummy;
	};

	constexpr RenderingParameter RenderingParamsInitialzer =
	{
		1.f,
		0.1f,
		3.f,
		1.3f,

		10.f,
		5.5f,
		50.f,
		24.f,

		{ 0.f, 0.18f, 0.3f },
		0.022f,

		{ 0.f, 0.18f, 0.3f },
		5.3f,

		1.f,
		60.f,
		4096.f,
		0.033f,

		1.f,
		1.f,
		0.9f,
		0.5f,

		0.11f,
		0.65f,
		0.5f,
		0.03f,

		0.7f,
		{1.f / 255.f, 11.f / 255.f, 19.f / 255.f},

		0.5f,
		1.f,
		{1.f, 1.f}
	};
	static_assert(sizeof(RenderingParameter) % 16 == 0, "CB 16byte alignment");
}