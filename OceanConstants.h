#pragma once
#include <array>

namespace ocean
{
	constexpr unsigned int CASCADE_COUNT = 4; // total 4 different wave cascade
	constexpr unsigned int N = 512; // fourier grid size, M = N

	constexpr unsigned int GROUP_X = N / 16;
	constexpr unsigned int GROUP_Y = N / 16;

	constexpr float DEFAULT_G = 9.8f;
	constexpr float DEFAULT_DEPTH = 1000.f;

	constexpr float WORLD_SCALER = 75.f;
	constexpr float SIMULATION_SIZE_IN_METER = 2048.f;

	struct InitialSpectrumWaveConstant
	{
		float g;
		float depth;
		float dummy[2];
	};
	static_assert(sizeof(InitialSpectrumWaveConstant) % 16 == 0, "CB 16byte alignment");

	constexpr InitialSpectrumWaveConstant InitialSpectrumWaveConstantInitializer =
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



	constexpr float CASCADE_1_L = 1023.f;
	constexpr float CASCADE_2_L = 255.f;
	constexpr float CASCADE_3_L = 17.f;
	constexpr float CASCADE_4_L = 3.f;

	constexpr float TEMP_DELTA_TIME = 0.03f;


	constexpr InitialSpectrumParameter LOCAL_CASCADE_1 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_1_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JONSWAP_CUTOFF_MIN, JonswapCutoffBoundary(CASCADE_2_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_2 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_2_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_2_L), JonswapCutoffBoundary(CASCADE_3_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_3 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_3_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_3_L), JonswapCutoffBoundary(CASCADE_4_L));
	constexpr InitialSpectrumParameter LOCAL_CASCADE_4 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_4_L, 1.f, DEFAULT_JONSWAP_WIND_DIR, JonswapCutoffBoundary(CASCADE_4_L), JONSWAP_CUTOFF_MAX);

	constexpr InitialSpectrumParameter SWELL_CASCADE_1 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_1_L, 0.3f, 0.f, JONSWAP_CUTOFF_MIN, JonswapCutoffBoundary(CASCADE_2_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_2 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_2_L, 0.3f, 0.f, JonswapCutoffBoundary(CASCADE_2_L), JonswapCutoffBoundary(CASCADE_3_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_3 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_3_L, 0.3f, 0.f, JonswapCutoffBoundary(CASCADE_3_L), JonswapCutoffBoundary(CASCADE_4_L));
	constexpr InitialSpectrumParameter SWELL_CASCADE_4 = JONSWAP_SPECTRUM_PARAMETER(CASCADE_4_L, 0.3f, 0.f, JonswapCutoffBoundary(CASCADE_4_L), JONSWAP_CUTOFF_MAX);

	constexpr std::array<CombineParameter, CASCADE_COUNT> CombineParameterInitializer =
	{ {
		{ CASCADE_1_L, 0.05f, 1.f, 0.f },
		{CASCADE_2_L, 0.1f, 1.f, 0.f},
		{CASCADE_3_L, 0.2f, 1.f, 0.f},
		{CASCADE_4_L, 0.3f, 1.f, 0.f},
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

}