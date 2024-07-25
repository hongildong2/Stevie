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

	ID3D11ShaderResourceView* GetNormalMapSRV() const;
	ID3D11ShaderResourceView* GetHeightMapSRV() const;
	float GetHeight(DirectX::SimpleMath::Vector3 worldPos) const;

private:
	bool mb_initialized;

	float m_heightMapCPU[ocean::N][ocean::N];

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_combineParamterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_combineParameterSRV;
	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> m_combineParameters;
	ocean::CombineWaveConstant m_combineWaveConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_combineWaveCB;

	// Result Normal Map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalMap;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_normalMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMapSRV;

	// Result Height Map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightMapGPU;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_heightMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_heightMapSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightMapGPUStaging; // staging texture for cpu

	// Texture for spectrum calculation
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_initialSpectrumMap; // tilde h0k, float2
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_initialSpectrumMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_initialSpectrumMapSRV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_initialSpectrumWaveCB;
	ocean::InitialSpectrumWaveConstant m_initialSpectrumWaveConstant;


	Microsoft::WRL::ComPtr<ID3D11Buffer> m_LocalInitialSpectrumParameterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_LocalInitialSpectrumParameterSRV;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_LocalInitialSpectrumParameters;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_SwellInitialSpectrumParameterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SwellInitialSpectrumParameterSRV;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_SwellInitialSpectrumParameters;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_waveVectorData; // [wave vector x, choppiness, wave vector z, frequency], float4
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_waveVectorDataUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_waveVectorDataSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_displacementMap; // time dependent spectrum, tilde h(k,t), float4 since x-y complex numbers
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_derivativeMap; // derivative of tilde h
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_displacementMapUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_derivativeMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_displacementMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_derivativeMapSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spectrumCB;
	ocean::SpectrumConstant m_spectrumConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_FFTCB;
	ocean::FFTConstant m_FFTConstant;
};

