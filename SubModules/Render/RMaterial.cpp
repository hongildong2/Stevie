#include "pch.h"
#include "RMaterial.h"
#include "RTexture.h"
#include "RShader.h"
#include "RSamplerState.h"
#include "GraphicsCommon1.h"

RMaterial::RMaterial(const IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState)
	: m_pRenderer(pRenderer)
	, m_pixelShader(pPixelShader)
	, m_blendState(pBlendState)
	, m_samplerStatesCount(0)
	, m_samplerStates{}
	, m_textureCount(0)
	, m_textures{}
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

void RMaterial::GetSamplerStates(void** ppOutSamplerStates, UINT* pOutSamplerStatesCount) const

{
	for (UINT i = 0; i < m_samplerStatesCount; ++i)
	{
		ppOutSamplerStates[i] = (void*)m_samplerStates[i];
	}
	*pOutSamplerStatesCount = m_samplerStatesCount;
}

void RMaterial::GetTextures(void** ppOutTextures, UINT* pOutTextureCount) const
{
	for (UINT i = 0; i < m_textureCount; ++i)
	{
		ppOutTextures[i] = (void*)m_textures[i];
	}
	*pOutTextureCount = m_textureCount;
}



RDemoMaterial::RDemoMaterial(const IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::DEMO_PS, nullptr)
{
	// assert pRenderer not null
	Initialize();
}


RSkyboxMaterial::RSkyboxMaterial(const IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::CUBEMAP_PS, nullptr)
{
	AddSamplerState(Graphics::LINEAR_CLAMP_SS);
}

void RSkyboxMaterial::Initialize()
{
	assert(m_textureCount == 1 && m_textures[0] != nullptr);
	RMaterial::Initialize();
}
