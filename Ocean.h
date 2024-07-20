#pragma once
#include "OceanConstants.h"

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
	bool mb_initialized;

	// Model m_oceanPlane;
	uint64_t m_heightMapCPU[ocean::CASCADE_COUNT][ocean::N][ocean::N]; // TODO : data type needs to be half-float4

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightMapGPU; // staging texture for cpu

	// Texture for spectrum calculation
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_initialSpectrumMap; // tilde h0k, float2
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_initialSpectrumMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_initialSpectrumMapSRV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_initialSpectrumWaveCB;
	ocean::InitialSpectrumWaveConstant m_initialSpectrumWaveConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_initialSpectrumParameterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_initialSpectrumParameterSRV;
	std::array<ocean::InitialSpectrumParameterConstant, 2 * ocean::CASCADE_COUNT> m_initialSpectrumParameterConstant; // even num is local wave, odd num is wind wave

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_waveVectorData; // [wave vector x, choppiness, wave vector z, frequency], float4
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_waveVectorDataUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_waveVectorDataSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spectrumMap; // time dependent spectrum, tilde h(k,t), float4 since x-y complex numbers
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_spectrumMapUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spectrumCB;
	ocean::SpectrumConstant m_spectrumConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_FFTCB;
	ocean::FFTConstant m_FFTConstant;
};

