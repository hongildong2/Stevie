#pragma once

struct InitialSpectrumWaveConstant
{
	float cutoffHigh;
	float cutoffLow;
	float g;
	float depth;
};
static_assert(sizeof(InitialSpectrumWaveConstant) % 16 == 0, "CB 16byte alignment");

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

struct SpectrumConstant
{
	float time;
	float dummy[3];
};
static_assert(sizeof(SpectrumConstant) % 16 == 0, "CB 16byte alignment");

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

	static const unsigned int GROUP_X = N / 16;
	static const unsigned int GROUP_Y = N / 16;

	bool mb_initialized;

	// Model m_oceanPlane;
	uint64_t m_heightMapCPU[CASCADE_COUNT][N][N]; // TODO : data type needs to be half-float4

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightMapGPU; // staging texture for cpu

	// Texture for spectrum calculation
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_initialSpectrumMap; // tilde h0k, float2
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_initialSpectrumMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_initialSpectrumMapSRV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_initialSpectrumWaveCB;
	InitialSpectrumWaveConstant m_initialSpectrumWaveConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_initialSpectrumParameterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_initialSpectrumParameterSRV;
	InitialSpectrumParameterConstant m_initialSpectrumParameterConstant[2 * CASCADE_COUNT]; // even num is local wave, odd num is wind wave

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_waveVectorData; // [wave vector x, choppiness, wave vector z, frequency], float4
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_waveVectorDataUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_waveVectorDataSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spectrumMap; // time dependent spectrum, tilde h(k,t), float4 since x-y complex numbers
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_spectrumMapUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spectrumCB;
	SpectrumConstant m_spectrumConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_FFTCB;
	FFTConstant m_FFTConstant;
};

