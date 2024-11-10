#pragma once
#include "pch.h"
#include "Core\OceanDefinition.h"

class ROceanMaterial final :
	public RMaterial
{
public:
	ROceanMaterial(const IRenderer* pRenderer);
	~ROceanMaterial();

	virtual void Initialize() override;
	void InitializeData();
	virtual void Update() override;

	void SetFoamTexture(RTexture* tex);
	void SetSkyTexture(RTexture* tex);

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;
	virtual void GetDisplacementTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;
private:
	RComputeShader* m_initialSpectrumCS;
	RComputeShader* m_timeDependentSpectrumCS;
	RComputeShader* m_FFTCS;
	RComputeShader* m_FFTPostProcessCS;
	RComputeShader* m_combineWaveCS;
	RComputeShader* m_foamSimulationCS;

	// TODO :: Structured Buffer

	RTexture* m_displacementTextureArray;
	RTexture* m_derivativeTextureArray;
	RTexture* m_turbulenceTextureArray;
	RTexture* m_waveVectorTextureArray;

	RTexture* m_initialSpectrumTextureArray;
	RTexture* m_heightTexture;

	RTexture* m_skyTexture;
	RTexture* m_foamTexture;

	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> m_combineParameters;
	ocean::CombineWaveConstant m_combineWaveConstant;

	ocean::OceanConfigurationConstant m_oceanConfigurationConstant;

	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_LocalInitialSpectrumParameters;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_SwellInitialSpectrumParameters;


	ocean::FFTConstant m_FFTParameter;
	ocean::SpectrumConstant m_spectrumParameter;
	ocean::RenderingParameter m_renderingParameter;
};

