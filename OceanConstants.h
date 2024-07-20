#pragma once
#include <array>

namespace ocean
{
	constexpr unsigned int CASCADE_COUNT = 4; // total 4 different wave cascade
	constexpr unsigned int N = 512; // fourier grid size, M = N
	constexpr unsigned int dx = 3; // grid size, in cm
	constexpr unsigned int L = N * dx; // total simulated size, in cm

	constexpr unsigned int GROUP_X = N / 16;
	constexpr unsigned int GROUP_Y = N / 16;

	constexpr float DEFAULT_G = 9.8f;

	struct InitialSpectrumWaveConstant
	{
		float cutoffHigh;
		float cutoffLow;
		float g;
		float depth;
	};
	static_assert(sizeof(InitialSpectrumWaveConstant) % 16 == 0, "CB 16byte alignment");

	constexpr InitialSpectrumWaveConstant InitialSpectrumWaveConstantInitializer =
	{
		1000000.f,
		0.f,
		9.8f,
		0.f // infinite depth
	};


	struct InitialSpectrumParameterConstant
	{
		float lengthScale; // simulation scale 0 ~ 1
		float scale; // wave scale
		float angle; // wind dir, in radian
		float spreadBlend; // 0 ~ 1
		float swell; // 0.01 ~ 1
		float alpha; // jonswap factor, default to 0.0081
		float peakOmega; // jonswap factor, typically default to 0.5
		float gamma; // peak enhancement, default to 3.3
		float shortWavesFade;
		float dummy[3];
	};
	static_assert(sizeof(InitialSpectrumParameterConstant) % 16 == 0, "CB 16byte alignment");

	constexpr float DEFAULT_JONSWAP_WIND_DIR = 0.f;
	constexpr float DEFAULT_JONSWAP_SPREAD_BLEND = 0.3f;
	constexpr float DEFAULT_JONSWAP_SWELL = 0.3f;
	constexpr float DEFAULT_JONSWAP_ALPHA = 0.0081f;
	constexpr float DEFAULT_JONSWAP_PEAK_OMEGA = 0.5f;
	constexpr float DEFAULT_JONSWAP_GAMMA = 3.3f;
	constexpr float DEFAULT_JONSWAP_WIND_SPEED = 10.f; // (m/s)
	constexpr float DEFAULT_JONSWAP_FADE = 0.1f;

	// https://dirsig.cis.rit.edu/docs/new/wavespectruminterface.html#:~:text=the%20JONSWAP%20spectrum%20is%20parametrized,100km)%20and%203.3%20%2C%20respectively.
	// default to 9km
	constexpr float DEFAULT_JONSWAP_FETCH_LENGTH = 9000.0f; // (m) 


	// しけいしぉ..いしぉ
	//float JonswapAlpha(float g, float fetch, float windSpeed)
	//{
	//	return 0.076f * std::pow(g * fetch / windSpeed / windSpeed, -0.22f);
	//}

	//float JonswapPeakFrequency(float g, float fetch, float windSpeed)
	//{
	//	return 22 * std::pow(windSpeed * fetch / g / g, -0.33f);
	//}


	constexpr InitialSpectrumParameterConstant blank = {};
	constexpr InitialSpectrumParameterConstant cascade1 = { 0.1f, 1.f ,DEFAULT_JONSWAP_WIND_DIR, DEFAULT_JONSWAP_SPREAD_BLEND, DEFAULT_JONSWAP_SWELL, DEFAULT_JONSWAP_ALPHA, DEFAULT_JONSWAP_PEAK_OMEGA, DEFAULT_JONSWAP_GAMMA, DEFAULT_JONSWAP_FADE, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade2 = { 0.3f, 1.f ,DEFAULT_JONSWAP_WIND_DIR, DEFAULT_JONSWAP_SPREAD_BLEND, DEFAULT_JONSWAP_SWELL, DEFAULT_JONSWAP_ALPHA, DEFAULT_JONSWAP_PEAK_OMEGA, DEFAULT_JONSWAP_GAMMA, DEFAULT_JONSWAP_FADE, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade3 = { 0.4f, 1.f ,DEFAULT_JONSWAP_WIND_DIR, DEFAULT_JONSWAP_SPREAD_BLEND, DEFAULT_JONSWAP_SWELL, DEFAULT_JONSWAP_ALPHA, DEFAULT_JONSWAP_PEAK_OMEGA, DEFAULT_JONSWAP_GAMMA, DEFAULT_JONSWAP_FADE, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade4 = { 0.7f, 1.f ,DEFAULT_JONSWAP_WIND_DIR, DEFAULT_JONSWAP_SPREAD_BLEND, DEFAULT_JONSWAP_SWELL, DEFAULT_JONSWAP_ALPHA, DEFAULT_JONSWAP_PEAK_OMEGA, DEFAULT_JONSWAP_GAMMA, DEFAULT_JONSWAP_FADE, {0.f} };



	constexpr std::array<InitialSpectrumParameterConstant, 2 * CASCADE_COUNT> InitialSpectrumParameterConstantInitializer =
	{
		cascade1, // even idx : local wave
		blank, // odd idx : wind wave
		cascade2,
		blank,
		cascade3,
		blank,
		cascade4,
		blank,
	};
	static_assert(sizeof(InitialSpectrumParameterConstantInitializer) / sizeof(InitialSpectrumParameterConstant) == 2 * CASCADE_COUNT, "CASCADE COUNT INCONSISTENT");

	struct FFTConstant
	{
		unsigned int targetCount;
		BOOL bDirection;
		BOOL bInverse;
		BOOL bScale;
		BOOL bPermute;
		BOOL dummy[3];
	};
	static_assert(sizeof(FFTConstant) % 16 == 0, "CB 16byte alignment");
	constexpr FFTConstant FFTConstantInitializer = { ocean::CASCADE_COUNT, false, true, false, false };

	struct SpectrumConstant
	{
		float time;
		float dummy[3];
	};
	static_assert(sizeof(SpectrumConstant) % 16 == 0, "CB 16byte alignment");
	constexpr SpectrumConstant SpectrumConstantInitializer = { 0.f , {0.f} };

}