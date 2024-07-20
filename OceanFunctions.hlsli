#ifndef __OCEAN_FUNCTIONS__
#define __OCEAN_FUNCTIONS__

#define SIZE 512 // 512 by 512 DFT
#define LOG_SIZE 9 // log2  512
#define TARGET_COUNT 4 // total 4 wave cascades

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

float DirectionSpectrum(float theta, float omega, SpectrumParameters pars)
{
	float s = SpreadPower(omega, pars.peakOmega)
		+ 16 * tanh(min(omega / pars.peakOmega, 20)) * pars.swell * pars.swell;
	return lerp(2 / 3.1415 * cos(theta) * cos(theta), Cosine2s(theta - pars.angle, s), pars.spreadBlend);
}

float ShortWavesFade(float kLength, SpectrumParameters pars)
{
	return exp(-pars.shortWavesFade * pars.shortWavesFade * kLength * kLength);
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
float JONSWAP(float omega, float g, float depth, SpectrumParameters pars)
{
	float sigma = omega <= pars.peakOmega ? 0.07 : 0.09;

	float r = exp(-(omega - pars.peakOmega) * (omega - pars.peakOmega)
		/ 2 / sigma / sigma / pars.peakOmega / pars.peakOmega);
	
	float oneOverOmega = 1 / omega;
	float peakOmegaOverOmega = pars.peakOmega / omega;
	return pars.scale * TMACorrection(omega, g, depth) * pars.alpha * g * g
		* oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega
		* exp(-1.25 * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega)
		* pow(abs(pars.gamma), r);
}

#endif /* __OCEN_FUNCTIONS__ */