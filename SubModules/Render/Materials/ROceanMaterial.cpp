#include "pch.h"
#include "ROceanMaterial.h"

ROceanMaterial::ROceanMaterial(IRenderer* pRenderer)
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
}

ROceanMaterial::~ROceanMaterial()
{
}

void ROceanMaterial::Initialize()
{
	MY_ASSERT(m_textures[SKY_TEXTURE] != nullptr);
	MY_ASSERT(m_textures[FOAM_TEXTURE] != nullptr);

	RTexture* displacementTextureArray = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	RTexture* derivativeTextureArray = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	RTexture* turbulenceTextureArray = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	RTexture* waveVectorTextureArray = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	RTexture* initialSpectrumTextureArray = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, ocean::CASCADE_COUNT, DXGI_FORMAT_R32G32_FLOAT);
	RTexture* heightTexture = m_pRenderer->CreateTexture2D(ocean::N, ocean::N, 1, DXGI_FORMAT_R32_FLOAT);


	m_combineParameterSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::CombineParameter), ocean::CASCADE_COUNT, &m_combineParameters);
	m_localInitialSpectrumSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_LocalInitialSpectrumParameters);
	m_swellInitialParameterSB = m_pRenderer->CreateStructuredBuffer(sizeof(ocean::InitialSpectrumParameter), ocean::CASCADE_COUNT, &m_SwellInitialSpectrumParameters);


	m_textures[DISPLACEMENT_TEXTURE2D_ARRAY] = displacementTextureArray;
	m_textures[DERIVATIVE_TEXTURE2D_ARRAY] = derivativeTextureArray;
	m_textures[TURBULENCE_TEXTURE2D_ARRAY] = turbulenceTextureArray;
	m_textures[WAVE_VECTOR_TEXTURE2D_ARRAY] = waveVectorTextureArray;

	m_textures[INITIAL_SPECTRUM_TEXTURE2D_ARRAY] = initialSpectrumTextureArray;
	m_textures[HEIGHT_TEXTURE2D] = heightTexture;

	InitializeData();
}

void ROceanMaterial::InitializeData()
{
	const RTexture* resultTextures[3] = { m_textures[INITIAL_SPECTRUM_TEXTURE2D_ARRAY], m_textures[WAVE_VECTOR_TEXTURE2D_ARRAY], m_textures[TURBULENCE_TEXTURE2D_ARRAY] };
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
		const RTexture* srcTextures[2] = { m_textures[INITIAL_SPECTRUM_TEXTURE2D_ARRAY], m_textures[WAVE_VECTOR_TEXTURE2D_ARRAY] };
		const RTexture* dstTextures[2] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_textures[DERIVATIVE_TEXTURE2D_ARRAY] };

		m_pRenderer->Compute(Graphics::OCEAN_TIME_DEPENDENT_SPECTRUM_CS, L"OCEAN TIME DEPENDENT COMPUTE", dstTextures, _countof(dstTextures), srcTextures, _countof(srcTextures), nullptr, 0, CAST_RENDER_PARAM_PTR(&m_spectrumParameter), ocean::GROUP_X, ocean::GROUP_Y, 1);
	}

	// Inverse FFT
	{
		const RTexture* displacementTexture[1] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY] };
		const RTexture* derivativeTexture[1] = { m_textures[DERIVATIVE_TEXTURE2D_ARRAY] };

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
		const RTexture* srcTextures[3] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_textures[DERIVATIVE_TEXTURE2D_ARRAY], m_combineParameterSB };
		const RTexture* dstTextures[1] = { m_textures[TURBULENCE_TEXTURE2D_ARRAY] };

		m_pRenderer->Compute(Graphics::OCEAN_FOAM_SIMULATION_CS, L"OCEAN FOAM SIMULATION COMPUTE", dstTextures, _countof(dstTextures), srcTextures, _countof(dstTextures), nullptr, 0, nullptr, ocean::GROUP_X, ocean::GROUP_Y, 1);
	}

	// Combine wave
	{
		const RTexture* srcTextures[2] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_combineParameterSB };
		const RTexture* dstTextures[1] = { m_textures[HEIGHT_TEXTURE2D] };
		const RSamplerState* ss[1] = { Graphics::LINEAR_WRAP_SS };

		m_pRenderer->Compute(Graphics::OCEAN_COMBINE_WAVE_CS, L"OCEAN COMBINE WAVE COMPUTE", dstTextures, _countof(dstTextures), srcTextures, _countof(srcTextures), ss, _countof(ss), CAST_RENDER_PARAM_PTR(&m_combineWaveConstant), ocean::GROUP_X, ocean::GROUP_Y, 1);

	}
}

void ROceanMaterial::SetFoamTexture(RTexture* tex)
{
	MY_ASSERT(tex != nullptr);
	m_textures[FOAM_TEXTURE] = tex;
}

void ROceanMaterial::SetSkyTexture(RTexture* tex)
{
	MY_ASSERT(tex != nullptr);
	m_textures[SKY_TEXTURE] = tex;
}

void ROceanMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	MEMCPY_RENDER_PARAM(pOutRenderParam, &m_renderingParameter);
}

void ROceanMaterial::GetTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	ppOutTextures[0] = m_textures[TURBULENCE_TEXTURE2D_ARRAY];
	ppOutTextures[1] = m_combineParameterSB;
	ppOutTextures[2] = m_textures[SKY_TEXTURE];
	ppOutTextures[3] = m_textures[FOAM_TEXTURE];

	*pOutTextureCount = 4;
}

void ROceanMaterial::GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	ppOutTextures[0] = m_textures[DISPLACEMENT_TEXTURE2D_ARRAY];
	ppOutTextures[1] = m_textures[DERIVATIVE_TEXTURE2D_ARRAY];
	ppOutTextures[2] = m_combineParameterSB;

	*pOutTextureCount = 3;
}
