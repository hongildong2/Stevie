#include "pch.h"

RMaterial::RMaterial(IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState)
	: m_pRenderer(pRenderer)
	, m_pixelShader(pPixelShader)
	, m_samplerStatesCount(0)
	, m_samplerStates{ nullptr, }
	, m_textureCount(0)
	, m_textures{ nullptr, }
	, m_bInitialized(false)
{
}

bool RMaterial::AddTexture(const RTexture* pTexture)
{
	if (m_textureCount >= MATERIAL_TEXTURE_MAX_COUNT || m_bInitialized == true)
	{
		return false;
	}

	m_textures[m_textureCount++] = pTexture;
	return true;
}

bool RMaterial::AddSamplerState(const RSamplerState* pSamplerState)
{
	if (m_samplerStatesCount >= MATERIAL_SAMPLE_STATE_MAX_COUNT || m_bInitialized == true)
	{
		return false;
	}

	m_samplerStates[m_samplerStatesCount++] = pSamplerState;
	return true;
}

void RMaterial::Initialize()
{
	m_bInitialized = true;
}

void RMaterial::Update()
{
	assert(m_bInitialized);
}

void RMaterial::GetSamplerStates(const RSamplerState** ppOutSamplerStates, UINT* pOutSamplerStatesCount) const

{
	for (UINT i = 0; i < m_samplerStatesCount; ++i)
	{
		ppOutSamplerStates[i] = m_samplerStates[i];
	}
	*pOutSamplerStatesCount = m_samplerStatesCount;
}

void RMaterial::GetTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	for (UINT i = 0; i < m_textureCount; ++i)
	{
		ppOutTextures[i] = m_textures[i];
	}
	*pOutTextureCount = m_textureCount;
}



RDemoMaterial::RDemoMaterial(IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::DEMO_PS, nullptr)
{
	// assert pRenderer not null
	Initialize();
}

void RDemoMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
}

void RDemoMaterial::GetDisplacementTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	pOutTextureCount = 0;
}


RSkyboxMaterial::RSkyboxMaterial(IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::CUBEMAP_PS, nullptr)
{
	AddSamplerState(Graphics::LINEAR_CLAMP_SS);
}

void RSkyboxMaterial::Initialize()
{
	MY_ASSERT(m_textureCount == 1 && m_textures[0] != nullptr);
	RMaterial::Initialize();
}

void RSkyboxMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
}

void RSkyboxMaterial::GetDisplacementTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	*pOutTextureCount = 0;
}

RBasicMaterial::RBasicMaterial(IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::BASIC_PS, nullptr)
	, m_constant(DEFAULT_MATERIAL)
{
	m_textureCount = BASIC_TEXTURE_INDEX::COUNT;
	AddSamplerState(Graphics::LINEAR_WRAP_SS);
	AddSamplerState(Graphics::LINEAR_CLAMP_SS);
}

void RBasicMaterial::GetMaterialConstant(RenderParam* pOutRenderParam) const
{
	MEMCPY_RENDER_PARAM(pOutRenderParam, &m_constant);
}

void RBasicMaterial::GetDisplacementTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const
{
	ppOutTextures[0] = m_textures[HEIGHT];
	*pOutTextureCount = 0;
}

void RBasicMaterial::SetAlbedoTexture(const RTexture* pAlbedoTexture)
{
	MY_ASSERT(pAlbedoTexture != nullptr);

	m_textures[ALBEDO] = pAlbedoTexture;
}

void RBasicMaterial::SetAOTexture(const RTexture* pAOTexture)
{
	MY_ASSERT(pAOTexture != nullptr);
	m_textures[AO] = pAOTexture;
}
void RBasicMaterial::SetHeightTexture(const RTexture* pHeightTexture)
{
	MY_ASSERT(pHeightTexture != nullptr);
	m_textures[HEIGHT] = pHeightTexture;
}
void RBasicMaterial::SetMetallicTexture(const RTexture* pMetallicTexture)
{
	MY_ASSERT(pMetallicTexture != nullptr);
	m_textures[METALLIC] = pMetallicTexture;
}
void RBasicMaterial::SetNormalTexture(const RTexture* pNormalTexture)
{
	MY_ASSERT(pNormalTexture != nullptr);
	m_textures[NORMAL] = pNormalTexture;
}
void RBasicMaterial::SetRoughnessTexture(const RTexture* pRoughnessTexture)
{
	MY_ASSERT(pRoughnessTexture != nullptr);
	m_textures[ROUGHNESS] = pRoughnessTexture;
}
void RBasicMaterial::SetEmissiveTexture(const RTexture* pEmissiveTexture)
{
	MY_ASSERT(pEmissiveTexture != nullptr);
	m_textures[EMISSIVE] = pEmissiveTexture;
}
void RBasicMaterial::SetOpacityTexture(const RTexture* pOpacityTexture)
{
	MY_ASSERT(pOpacityTexture != nullptr);
	m_textures[OPACITY] = pOpacityTexture;
}

void RBasicMaterial::Initialize()
{
	m_constant.bUseAlbedoTexture = m_textures[ALBEDO] != nullptr;
	m_constant.bUseAOTexture = m_textures[AO] != nullptr;
	m_constant.bUseHeightTexture = m_textures[HEIGHT] != nullptr;
	m_constant.bUseMetallicTexture = m_textures[METALLIC] != nullptr;

	m_constant.bUseNormalTexture = m_textures[NORMAL] != nullptr;
	m_constant.bUseRoughnessTexture = m_textures[ROUGHNESS] != nullptr;
	m_constant.bUseEmissiveTexture = m_textures[EMISSIVE] != nullptr;
	m_constant.bUseOpacityTexture = m_textures[OPACITY] != nullptr;

	m_bInitialized = TRUE;
}
