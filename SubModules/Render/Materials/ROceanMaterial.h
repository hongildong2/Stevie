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
	virtual void GetTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;
	virtual void GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;
private:
	enum
	{
		DISPLACEMENT_TEXTURE2D_ARRAY,
		DERIVATIVE_TEXTURE2D_ARRAY,
		TURBULENCE_TEXTURE2D_ARRAY,
		WAVE_VECTOR_TEXTURE2D_ARRAY,
		INITIAL_SPECTRUM_TEXTURE2D_ARRAY,
		HEIGHT_TEXTURE2D,
		SKY_TEXTURE,
		FOAM_TEXTURE
	};

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

