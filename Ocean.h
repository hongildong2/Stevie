#pragma once
#include "pch.h"

#include "OceanConstants.h"
#include "Model.h"
class Ocean final : public Model
{
public:
	Ocean();
	virtual ~Ocean() = default;
	Ocean(const Ocean& other) = delete;
	Ocean& operator=(const Ocean& other) = delete;

	const AObject* GetThis() const override;

	virtual void Initialize(ID3D11Device1* pDevice) override;
	void InitializeData(ID3D11DeviceContext1* context);

	virtual void Update(ID3D11DeviceContext1* context) override; // is timer necessary?
	virtual void Render(ID3D11DeviceContext1* pContext) override;
	virtual void RenderOverride(ID3D11DeviceContext1* pContext, const GraphicsPSO& pso) override;

	inline ID3D11ShaderResourceView* GetDisplacementMapsSRV() const
	{
		return m_displacementMapSRV.Get();
	}

	inline ID3D11ShaderResourceView* GetDerivativeMapsSRV() const
	{
		return m_derivativeMapSRV.Get();
	}

	inline ID3D11ShaderResourceView* GetCombineParameterSRV() const
	{
		return m_combineParameterSRV.Get();
	}

	inline ID3D11ShaderResourceView* GetTurbulenceMapsSRV() const
	{
		return m_turbulenceMapSRV.Get();
	}

	inline ID3D11Buffer* GetCombineWaveCB() const
	{
		return m_combineWaveCB.Get();
	}

	float GetHeight(DirectX::SimpleMath::Vector2 XZ) const;

private:
	bool mb_initialized;

	float m_heightMapCPU[ocean::N][ocean::N];

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_combineParamterSB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_combineParameterSRV;
	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> m_combineParameters;
	ocean::CombineWaveConstant m_combineWaveConstant;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_combineWaveCB;

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
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_turbulenceMap;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_displacementMapUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_derivativeMapUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_turbulenceMapUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_displacementMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_derivativeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_turbulenceMapSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spectrumCB;
	ocean::SpectrumConstant m_spectrumConstant;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_FFTCB;
	ocean::FFTConstant m_FFTConstant;
};

