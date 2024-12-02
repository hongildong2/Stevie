#include "pch.h"
#include "RCloudMaterial.h"

constexpr unsigned int CLOUD_TEX_SIZE = 512;


RCloudMaterial::RCloudMaterial(IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::VOLUME_PS)
	, m_densityTexture3D(nullptr)
	, m_lightingTexture3D(nullptr)
{
	m_geometrySamplerStatesCount = 0;
	m_geometryTexturesCount = 0;
	m_pixelSamplerStatesCount = 1;
	m_pixelSamplerStates[0] = Graphics::LINEAR_CLAMP_SS;
}

RCloudMaterial::~RCloudMaterial()
{
}

void RCloudMaterial::Initialize()
{
	m_densityTexture3D = m_pRenderer->CreateTexture3D(CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, DXGI_FORMAT_R16_FLOAT);
	m_lightingTexture3D = m_pRenderer->CreateTexture3D(CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, CLOUD_TEX_SIZE, DXGI_FORMAT_R16_FLOAT);

	m_pixelTextures[0] = m_densityTexture3D;
	m_pixelTextures[1] = m_lightingTexture3D;
	m_pixelTexturesCount = 2;
	InitializeData();
}

void RCloudMaterial::InitializeData()
{
	const RTexture* densityTexture[1] = { m_densityTexture3D };
	const RSamplerState* ss[1] = { Graphics::LINEAR_CLAMP_SS };
	m_pRenderer->Compute(Graphics::CLOUD_DENSITY_CS, L"CLOUD DENSITY VOLUME INIT COMPUTE", densityTexture, _countof(densityTexture), nullptr, 0, ss, _countof(ss), nullptr, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 4);

	const RTexture* lightingTexture[1] = { m_lightingTexture3D };
	m_pRenderer->Compute(Graphics::CLOUD_LIGHTING_CS, L"CLOUD LIGHT VOLUME INIT COMPUTE", lightingTexture, _countof(lightingTexture), densityTexture, _countof(densityTexture), ss, _countof(ss), nullptr, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 16, CLOUD_TEX_SIZE / 4);

	m_bInitialized = true;
}

void RCloudMaterial::Update()
{
	if (m_bInitialized == false)
	{
		InitializeData();
	}
}

void RCloudMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	pOutRenderParam;
}
