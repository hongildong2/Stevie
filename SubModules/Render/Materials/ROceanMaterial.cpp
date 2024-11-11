#include "pch.h"
#include "ROceanMaterial.h"

ROceanMaterial::ROceanMaterial(IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::OCEAN_SURFACE_PS, Graphics::ALPHA_BS)
	, m_combineWaveConstant(ocean::CombineWaveConstantInitializer)
	, m_combineParameters(ocean::CombineParameterInitializer)
	, m_FFTParameter(ocean::FFTConstantInitializer)
	, m_LocalInitialSpectrumParameters(ocean::LocalInitialSpectrumParameterInitializer)
	, m_SwellInitialSpectrumParameters(ocean::SwellInitialSpectrumParameterConstantInitializer)
	, m_spectrumParameter(ocean::SpectrumConstantInitializer)
	, m_oceanConfigurationConstant(ocean::OceanConfigurationInitializer)
	, m_renderingParameter(ocean::RenderingParamsInitialzer)
{
	// Add Sampler States
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



	// TODO :: 
	RStructuredBuffer* combineParameterSB = nullptr;
	RStructuredBuffer* localInitialSpectrumSB = nullptr;
	RStructuredBuffer* swellInitialParameterSB = nullptr;


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
	const RTexture* srcTextures[1] = { m_localInitialSpectrumSB };
	m_pRenderer->Compute(Graphics::OCEAN_INITIAL_SPECTRUM_CS, resultTextures, 3, srcTextures, 1, m_samplerStates, ? ? , &m_oceanConfigurationConstant, ocean::GROUP_X, ocean::GROUP_Y, 1);

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
		const RTexture* dstTextures[2] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_textures[DERIVATIVE_TEXTURE2D_ARRAY] };
		const RTexture* srcTextures[2] = { m_textures[INITIAL_SPECTRUM_TEXTURE2D_ARRAY], m_textures[WAVE_VECTOR_TEXTURE2D_ARRAY] };

		m_pRenderer->Compute(Graphics::OCEAN_TIME_DEPENDENT_SPECTRUM_CS, dstTextures, 2, srcTextures, 2, ? ? , ? ? , &m_spectrumParameter, ocean::GROUP_X, ocean::GROUP_Y, 1);
	}

	// Inverse FFT
	{
		const RTexture* displacementTexture[1] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY] };
		const RTexture* derivativeTexture[1] = { m_textures[DERIVATIVE_TEXTURE2D_ARRAY] };

		// Horizontal IFFT
		{
			m_FFTParameter.bInverse = TRUE;
			m_FFTParameter.bDirection = FALSE;

			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, displacementTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, 1, ocean::N, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, derivativeTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, 1, ocean::N, 1);

		}

		// Vertical IFFT
		{
			m_FFTParameter.bDirection = TRUE;
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, displacementTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, 1, ocean::N, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, derivativeTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, 1, ocean::N, 1);
		}

		// PostProcess
		{
			m_FFTParameter.bPermute = TRUE;
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, displacementTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, ocean::GROUP_X, ocean::GROUP_Y, 1);
			m_pRenderer->Compute(Graphics::OCEAN_FFT_CS, derivativeTexture, 1, nullptr, 0, ? ? , ? ? , &m_FFTParameter, ocean::GROUP_X, ocean::GROUP_Y, 1);
		}
	}

	// Foam Simulation
	{
		const RTexture* srcTextures[3] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_textures[DERIVATIVE_TEXTURE2D_ARRAY], m_combineParameterSB };
		const RTexture* dstTextures[1] = { m_textures[TURBULENCE_TEXTURE2D_ARRAY] };

		m_pRenderer->Compute(Graphics::OCEAN_FOAM_SIMULATION_CS, dstTextures, 1, srcTextures, 3, ? ? , ? ? , nullptr, ocean::GROUP_X, ocean::GROUP_Y, 1);
	}

	// Combine wave
	{
		const RTexture* srcTextures[2] = { m_textures[DISPLACEMENT_TEXTURE2D_ARRAY], m_combineParameterSB };
		const RTexture* dstTextures[1] = { m_textures[HEIGHT_TEXTURE2D] };

		m_pRenderer->Compute(Graphics::OCEAN_COMBINE_WAVE_CS, dstTextures, 1, srcTextures, 2, ? ? , ? ? , &m_combineWaveConstant, ocean::GROUP_X, ocean::GROUP_Y, 1);

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
	std::memcpy(pOutRenderParam, &m_renderingParameter, sizeof(ocean::RenderingParameter));
}

void ROceanMaterial::GetTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	ppOutTextures[0] = m_textures[TURBULENCE_TEXTURE2D_ARRAY];
	ppOutTextures[1] = m_combineParameterSB;
	ppOutTextures[2] = m_textures[SKY_TEXTURE];
	ppOutTextures[3] = m_textures[FOAM_TEXTURE];

	*pOutTextureCount = 4;
}

void ROceanMaterial::GetDisplacementTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	ppOutTextures[0] = m_textures[DISPLACEMENT_TEXTURE2D_ARRAY];
	ppOutTextures[1] = m_textures[DERIVATIVE_TEXTURE2D_ARRAY];
	ppOutTextures[2] = m_combineParameterSB;

	*pOutTextureCount = 3;
}
