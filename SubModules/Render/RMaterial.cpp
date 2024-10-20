#include "pch.h"
#include "RMaterial.h"
#include "RTexture.h"
#include "RShader.h"
#include "RSamplerState.h"
#include "GraphicsCommon1.h"

RMaterial::RMaterial(const IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState, const RSamplerState* const* ppSamplerStates, const UINT samplerStatesCount)
	: m_pRenderer(pRenderer)
	, m_pixelShader(pPixelShader)
	, m_blendState(pBlendState)
	, m_samplerStatesCount(samplerStatesCount)
	, m_textureCount(0)
	, m_textures{}
	, m_bInitialized(false)
{
	// TODO :: assert(samplerStatesCount < MAX);
	for (UINT i = 0; i < m_samplerStatesCount; ++i)
	{
		m_samplerStates[i] = ppSamplerStates[i];
	}
}

bool RMaterial::AddTexture(const RTexture* pTexture)
{
	if (m_textureCount >= MATERIAL_MAX_TEXTURE_SLOT || m_bInitialized == true)
	{
		return false;
	}

	m_textures[m_textureCount++] = pTexture;
	return true;
}

void RMaterial::Initialize()
{
	m_bInitialized = true;
}

void RMaterial::Update()
{
	// assert initialized
}

RDemoMaterial::RDemoMaterial(const IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::DEMO_PS, nullptr, nullptr, 0)
{
	// assert pRenderer not null
	Initialize();
}


const static RSamplerState* SKYBOX_SS_INITIALIZER[1] = { Graphics::LINEAR_CLAMP_SS };
RSkyboxMaterial::RSkyboxMaterial(const IRenderer* pRenderer)
	: RMaterial(pRenderer, Graphics::CUBEMAP_PS, nullptr, SKYBOX_SS_INITIALIZER, 1)
{
}

void RSkyboxMaterial::Initialize()
{
	assert(m_textureCount == 1 && m_textures[0] != nullptr);
	RMaterial::Initialize();
}
