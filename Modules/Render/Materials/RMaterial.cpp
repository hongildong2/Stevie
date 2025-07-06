#include "pch.h"

RMaterial::RMaterial(RRenderer* pRenderer, const RPixelShader* pPixelShader)
	: m_pRenderer(pRenderer)
	, m_pixelShader(pPixelShader)
	, m_geometrySamplerStatesCount(0)
	, m_geometrySamplerStates{ nullptr, }
	, m_geometryTexturesCount(0)
	, m_geometryTextures{ nullptr, }
	, m_pixelSamplerStatesCount(0)
	, m_pixelSamplerStates{ nullptr, }
	, m_pixelTexturesCount(0)
	, m_pixelTextures{ nullptr, }
	, m_bInitialized(false)
	, m_bIsHeightMapped(false)
{
}


void RMaterial::Initialize()
{
	m_bInitialized = true;
}

void RMaterial::Update()
{
	assert(m_bInitialized);
}

void RMaterial::GetGeometryTextures(const RTexture** ppOutTextures) const
{
	MY_ASSERT(m_bInitialized == TRUE);
	for (UINT i = 0; i < m_geometryTexturesCount; ++i)
	{
		MY_ASSERT(m_geometryTextures[i] != nullptr);
		ppOutTextures[i] = m_geometryTextures[i];
	}
}

void RMaterial::GetGeometrySamplerStates(const RSamplerState** ppOutSamplerStates) const
{
	MY_ASSERT(m_bInitialized == TRUE);
	for (UINT i = 0; i < m_geometrySamplerStatesCount; ++i)
	{
		MY_ASSERT(m_geometrySamplerStates[i] != nullptr);
		ppOutSamplerStates[i] = m_geometrySamplerStates[i];
	}
}

void RMaterial::GetPixelTextures(const RTexture** ppOutTextures) const
{
	MY_ASSERT(m_bInitialized == TRUE);
	for (UINT i = 0; i < m_pixelTexturesCount; ++i)
	{
		ppOutTextures[i] = m_pixelTextures[i];
	}
}

void RMaterial::GetPixelSamplerStates(const RSamplerState** ppOutSamplerStates) const
{
	MY_ASSERT(m_bInitialized == TRUE);
	for (UINT i = 0; i < m_pixelSamplerStatesCount; ++i)
	{
		MY_ASSERT(m_pixelSamplerStates[i] != nullptr);
		ppOutSamplerStates[i] = m_pixelSamplerStates[i];
	}
}


RSkyboxMaterial::RSkyboxMaterial(RRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::CUBEMAP_PS)
{
	m_geometryTexturesCount = 0;
	m_geometrySamplerStatesCount = 0;


	m_pixelSamplerStates[0] = Graphics::LINEAR_CLAMP_SS;
	m_pixelSamplerStatesCount = 1;
}

void RSkyboxMaterial::Initialize()
{
	RMaterial::Initialize();
}

void RSkyboxMaterial::SetSkyboxTexture(const RTexture* pTex)
{
	m_pixelTextures[0] = pTex;
	m_pixelTexturesCount = 1;
}

void RSkyboxMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	pOutRenderParam;
}

RBasicMaterial::RBasicMaterial(RRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::BASIC_PS)
	, m_constant(DEFAULT_MATERIAL)
{
	m_pixelSamplerStates[0] = Graphics::LINEAR_WRAP_SS;
	m_pixelSamplerStates[1] = Graphics::LINEAR_CLAMP_SS;
	m_pixelSamplerStatesCount = 2;

	m_geometrySamplerStates[0] = Graphics::LINEAR_WRAP_SS;
	m_geometrySamplerStatesCount = 1;

}

void RBasicMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	MEMCPY_RENDER_PARAM(pOutRenderParam, &m_constant);
}


void RBasicMaterial::SetAlbedoTexture(const RTexture* pAlbedoTexture)
{
	MY_ASSERT(pAlbedoTexture != nullptr);

	m_pixelTextures[PIXEL_TEX_SLOTS::ALBEDO] = pAlbedoTexture;
}

void RBasicMaterial::SetAOTexture(const RTexture* pAOTexture)
{
	MY_ASSERT(pAOTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::AO] = pAOTexture;
}
void RBasicMaterial::SetHeightTexture(const RTexture* pHeightTexture)
{
	MY_ASSERT(pHeightTexture != nullptr);
	m_bIsHeightMapped = TRUE;
	m_geometryTextures[GEOMETRY_TEX_SLOTS::HEIGHT] = pHeightTexture;
	m_geometryTexturesCount = 1;
}
void RBasicMaterial::SetMetallicTexture(const RTexture* pMetallicTexture)
{
	MY_ASSERT(pMetallicTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::METALLIC] = pMetallicTexture;
}
void RBasicMaterial::SetNormalTexture(const RTexture* pNormalTexture)
{
	MY_ASSERT(pNormalTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::NORMAL] = pNormalTexture;
}
void RBasicMaterial::SetRoughnessTexture(const RTexture* pRoughnessTexture)
{
	MY_ASSERT(pRoughnessTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::ROUGHNESS] = pRoughnessTexture;
}
void RBasicMaterial::SetEmissiveTexture(const RTexture* pEmissiveTexture)
{
	MY_ASSERT(pEmissiveTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::EMISSIVE] = pEmissiveTexture;
}
void RBasicMaterial::SetOpacityTexture(const RTexture* pOpacityTexture)
{
	MY_ASSERT(pOpacityTexture != nullptr);
	m_pixelTextures[PIXEL_TEX_SLOTS::OPACITY] = pOpacityTexture;
}

void RBasicMaterial::Initialize()
{
	m_constant.bUseAlbedoTexture = m_pixelTextures[PIXEL_TEX_SLOTS::ALBEDO] != nullptr;
	m_constant.bUseAOTexture = m_pixelTextures[PIXEL_TEX_SLOTS::AO] != nullptr;
	m_constant.bUseHeightTexture = m_geometryTextures[GEOMETRY_TEX_SLOTS::HEIGHT] != nullptr;
	m_constant.bUseMetallicTexture = m_pixelTextures[PIXEL_TEX_SLOTS::METALLIC] != nullptr;

	m_constant.bUseNormalTexture = m_pixelTextures[PIXEL_TEX_SLOTS::NORMAL] != nullptr;
	m_constant.bUseRoughnessTexture = m_pixelTextures[PIXEL_TEX_SLOTS::ROUGHNESS] != nullptr;
	m_constant.bUseEmissiveTexture = m_pixelTextures[PIXEL_TEX_SLOTS::EMISSIVE] != nullptr;
	m_constant.bUseOpacityTexture = m_pixelTextures[PIXEL_TEX_SLOTS::OPACITY] != nullptr;

	m_pixelTexturesCount = PIXEL_TEX_SLOTS::COUNT;

	m_bInitialized = TRUE;
}
