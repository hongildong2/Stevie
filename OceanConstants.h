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
		2.f
	};


	struct InitialSpectrumParameterConstant
	{
		float lengthScale;
		float scale;
		float angle;
		float spreadBlend;
		float swell;
		float alpha;
		float peakOmega;
		float gamma;
		float shortWavesFade;
		float dummy[3];
	};
	static_assert(sizeof(InitialSpectrumParameterConstant) % 16 == 0, "CB 16byte alignment");

	constexpr InitialSpectrumParameterConstant blank = {};
	constexpr InitialSpectrumParameterConstant cascade1 = { 1.f, 1.f ,0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade2 = { 3.f, 1.f ,0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade3 = { 6.f, 1.f ,0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, {0.f} };
	constexpr InitialSpectrumParameterConstant cascade4 = { 9.f, 1.f ,0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, {0.f} };

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