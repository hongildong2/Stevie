#pragma once
#include "pch.h"
#include "Core\OceanDefinition.h"

class ROceanMaterial final :
	public RMaterial
{
public:
	ROceanMaterial(IRenderer* pRenderer);
	~ROceanMaterial();

	virtual void Initialize() override;
	void InitializeData();
	virtual void Update() override;

	void SetFoamTexture(RTexture* tex);
	void SetSkyTexture(RTexture* tex);

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;
private:
	RTexture* m_displacementTex;
	RTexture* m_derivativeTex;
	RTexture* m_turbulenceTex;
	RTexture* m_waveVectorTex;
	RTexture* m_initSpectrumTex;
	RTexture* m_skyTex;
	RTexture* m_foamTex;

	RTexture* m_combineParameterSB;
	RTexture* m_localInitialSpectrumSB;
	RTexture* m_swellInitialParameterSB;


	std::array<ocean::CombineParameter, ocean::CASCADE_COUNT> m_combineParameters;
	ocean::CombineWaveConstant m_combineWaveConstant;

	ocean::OceanConfigurationConstant m_oceanConfigurationConstant;

	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_LocalInitialSpectrumParameters;
	std::array<ocean::InitialSpectrumParameter, ocean::CASCADE_COUNT> m_SwellInitialSpectrumParameters;


	ocean::FFTConstant m_FFTParameter;
	ocean::SpectrumConstant m_spectrumParameter;
	ocean::RenderingParameter m_renderingParameter;
};

