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

	void Initialize(ID3D11DeviceContext1* context);
	void Update(ID3D11DeviceContext1* context); // is timer necessary?
	void Draw(ID3D11DeviceContext1* context);

private:
	static const unsigned int CASCADE_COUNT = 4; // total 4 different wave cascade
	static const unsigned int N = 512; // fourier grid size, M = N
	static const unsigned int dx = 3; // grid size, in cm
	static const unsigned int L = N * dx; // total simulated size, in cm
	bool mb_initialized;

	// Model m_oceanPlane;
	double m_heightMapCPU[N][N];

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightMap; // staging texture for cpu

	// Texture for spectrum calculation
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_initialSpectrumMap; // tilde h0k, float2
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_initialSpectrumMapUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_waveVectorData; // [wave vector x, choppiness, wave vector z, frequency], float4
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_waveVectorDataUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spectrumMap; // time dependent spectrum, tilde h(k,t), float4 since x-y complex numbers
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_spectrumMapUAV;


};

