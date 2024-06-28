#pragma once

struct SpectrumParameters
{
	float scale;
	float angle;
	float spreadBlend;
	float swell;
	float alpha;
	float peakOmega;
	float gamma;
	float shortWavesFade;
};


class Ocean
{
public:
	Ocean(ID3D11Device1* device);
	Ocean(const Ocean& other) = delete;
	Ocean& operator=(const Ocean& other) = delete;
	void InitData(ID3D11DeviceContext1* context);
	void Update(ID3D11DeviceContext1* context); // is timer necessary?
	void Draw(ID3D11DeviceContext1* context);

private:
	// Texture3D, z for each wave cascade
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_heightMap;

	// Texture for spectrum calculation
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_initialSpectrumMap; // tilde h0k
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_conjugateInitialSpectrumMap; // *h0k
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_waveVectorData; // wave vector x, chop, wave vector z, frequency
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_spectrumMap; // time dependent spectrum, tilde h(k,t)


	static const unsigned int CASCADE_COUNT = 4; // total 4 different wave cascade
	static const unsigned int N = 512; // fourier grid size, M = N
	static const unsigned int dx = 3; // grid size, in cm
	static const unsigned int L = N * dx; // total simulated size, in cm
};

