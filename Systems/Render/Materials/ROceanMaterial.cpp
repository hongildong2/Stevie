#include "pch.h"
#include "ROceanMaterial.h"

ROceanMaterial::ROceanMaterial(RRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::OCEAN_SURFACE_PS)
	, m_combineWaveConstant(ocean::CombineWaveConstantInitializer)
	, m_combineParameters(ocean::CombineParameterInitializer)
	, m_FFTParameter(ocean::FFTConstantInitializer)
	, m_LocalInitialSpectrumParameters(ocean::LocalInitialSpectrumParameterInitializer)
	, m_SwellInitialSpectrumParameters(ocean::SwellInitialSpectrumParameterConstantInitializer)
	, m_spectrumParameter(ocean::SpectrumConstantInitializer)
	, m_oceanConfigurationConstant(ocean::OceanConfigurationInitializer)
	, m_renderingParameter(ocean::RenderingParamsInitialzer)
	, m_combineParameterSB(nullptr)
	, m_localInitialSpectrumSB(nullptr)
	, m_swellInitialParameterSB(nullptr)
{
	m_bIsHeightMapped = TRUE;

	m_pixelSamplerStates[0] = Graphics::LINEAR_WRAP_SS;
	m_pixelSamplerStatesCount = 1;
	m_geometrySamplerStates[0] = Graphics::LINEAR_WRAP_SS;
	m_geometrySamplerStatesCount = 1;
}

ROceanMaterial::~ROceanMaterial()
{
}

void ROceanMaterial::Initialize()
{
	MY_ASSERT(m_skyTex != nullptr);
	MY_ASSERT(m_foamTex != nullptr);

	m_displacementTex = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_derivativeTex = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_turbulenceTex = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_waveVectorTex = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_initSpectrumTex = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32_FLOAT);

	m_combineParameterSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::CombineParameter), ocean::CASCADE_COUNT, &m_combineParameters);
	m_localInitialSpectrumSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_LocalInitialSpectrumParameters);
	m_swellInitialParameterSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_SwellInitialSpectrumParameters);

	m_pixelTextures[0] = m_turbulenceTex;
	m_pixelTextures[1] = m_combineParameterSB;
	m_pixelTextures[2] = m_skyTex;
	m_pixelTextures[3] = m_foamTex;
	m_pixelTexturesCount = 4;

	m_geometryTextures[0] = m_displacementTex;
	m_geometryTextures[1] = m_derivativeTex;
	m_geometryTextures[2] = m_combineParameterSB;
	m_geometryTexturesCount = 3;

	InitializeData();
}

void ROceanMaterial::InitializeData()
{
	const RTexture* resultTextures[3] = { m_initSpectrumTex , m_waveVectorTex, m_turbulenceTex };
	const RTexture* srcTextures[2] = { m_localInitialSpectrumSB, m_swellInitialParameterSB };
	m_pRenderer->Compute(Graphics::OCEAN_INITIAL_SPECTRUM_CS, L"OCEAN INITIAL SPECTRUM COMPUTE", resultTextures, _countof(resultTextures), srcTextures, _countof(srcTextures), nullptr, 0, CAST_RENDER_PARAM_PTR(&m_oceanConfigurationConstant), ocean::GROUP_X, ocean::GROUP_Y, 1);

	m_bInitialized = true;
}

void ROceanMaterial::Update()
{
	if (m_bInitialized == false)
	{
		InitializeData();
	}

	// TEMP
	m_spectrumParameter.time += ocean::TEMP_DELTA_TIME;

	// Timedepedent Spectrum, from InitialSpectrum
	{
		const RTexture* srcTextures[2] = { m_initSpectrumTex, m_waveVectorTex };
		const RTexture* dstTextures[2] = { m_displacementTex, m_derivativeTex };

		m_pRenderer->Compute(Graphics::OCEAN_TIME_DEPENDENT_SPECTRUM_CS, L"OCEAN TIME DEPENDENT COMPUTE", dstTextures, _countof(dstTextures), srcTextures, _countof(srcTextures), nullptr, 0, CAST_RENDER_PARAM_PTR(&m_spectrumParameter), ocean::GROUP_X, ocean::GROUP_Y, 1);
	}

	// Inverse FFT
	{
		const RTexture* displacementTexture[1] = { m_displacementTex };
		const RTexture* derivativeTexture[1] = { m_derivativeTex };

		// Horizontal IFFT
		{
			m_FFTParameter.bInverse = TRUE;
			m_FFTParameter.bDirection = FALSE;

			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, L"OCEAN DISPLACEMENT MAP HORIZONTAL FFT", displacementTexture, _countof(displacementTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), 1, ocean::N, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, L"OCEAN DERIVATIVE MAP HORIZONTAL FFT", derivativeTexture, _countof(derivativeTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), 1, ocean::N, 1);

		}

		// Vertical IFFT
		{
			m_FFTParameter.bDirection = TRUE;
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, L"OCEAN DISPLACEMENT MAP VERTICAL FFT", displacementTexture, _countof(displacementTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), 1, ocean::N, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, L"OCEAN DERIVATIVE MAP VERTICAL FFT", derivativeTexture, _countof(derivativeTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), 1, ocean::N, 1);
		}

		// PostProcess
		{
			m_FFTParameter.bPermute = TRUE;
			m_pRenderer->Compute(Graphics::OCEAN_FFT_POST_PROCESS_CS, L"OCEAN DISPLACEMENT MAP FFT POST PROCESS", displacementTexture, _countof(displacementTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), ocean::GROUP_X, ocean::GROUP_Y, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_POST_PROCESS_CS, L"OCEAN DERIVATIVE MAP FFT POST PROCESS", derivativeTexture, _countof(derivativeTexture), nullptr, 0, nullptr, 0, CAST_RENDER_PARAM_PTR(&m_FFTParameter), ocean::GROUP_X, ocean::GROUP_Y, 1);
		}
	}


	// Foam Simulation
	{
		const RTexture* srcTextures[3] = { m_displacementTex, m_derivativeTex, m_combineParameterSB };
		const RTexture* dstTextures[1] = { m_turbulenceTex };

		m_pRenderer->Compute(Graphics::OCEAN_FOAM_SIMULATION_CS, L"OCEAN FOAM SIMULATION COMPUTE", dstTextures, _countof(dstTextures), srcTextures, _countof(dstTextures), nullptr, 0, nullptr, ocean::GROUP_X, ocean::GROUP_Y, 1);
	}
}

void ROceanMaterial::SetFoamTexture(RTexture* tex)
{
	MY_ASSERT(tex != nullptr);
	m_foamTex = tex;
}

void ROceanMaterial::SetSkyTexture(RTexture* tex)
{
	MY_ASSERT(tex != nullptr);
	m_skyTex = tex;
}

void ROceanMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	MEMCPY_RENDER_PARAM(pOutRenderParam, &m_renderingParameter);
}
