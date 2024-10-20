#pragma once
#include "pch.h"
class IRenderer;
class RTexture;
class RTextureCube;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;


constexpr UINT MATERIAL_MAX_TEXTURE_SLOT = 10;
constexpr UINT MATERIAL_MAX_SAMPLER_STATE_SLOT = 10;
class RMaterial
{
public:
	RMaterial(const IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState, const RSamplerState* const* ppSamplerStates, const UINT samplerStatesCount);
	virtual ~RMaterial() = default;

	bool AddTexture(const RTexture* pTexture);

	virtual void Initialize();
	virtual void Update();

	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}
	inline const RBlendState* GetBlendState() const
	{
		return m_blendState;
	}
	inline const RSamplerState* const* GetSamplerStates() const
	{
		return m_samplerStates;
	}
	inline const UINT GetSamplerStatesCount() const
	{
		return m_samplerStatesCount;
	}
	inline const UINT GetTexturesCount() const
	{
		return m_textureCount;
	}
	inline const RTexture* const* GetTextures() const
	{
		return m_textures;
	}

protected:
	const IRenderer* m_pRenderer;
	const RPixelShader* m_pixelShader;
	const RBlendState* m_blendState;

	const RSamplerState* m_samplerStates[MATERIAL_MAX_SAMPLER_STATE_SLOT];
	const UINT m_samplerStatesCount;

	const RTexture* m_textures[MATERIAL_MAX_TEXTURE_SLOT];
	UINT m_textureCount;

	bool m_bInitialized;
};

class RDemoMaterial final : public RMaterial
{
public:
	RDemoMaterial(const IRenderer* pRenderer);
	~RDemoMaterial() = default;
};


class RSkyboxMaterial final : public RMaterial
{
public:
	RSkyboxMaterial(const IRenderer* pRenderer);
	~RSkyboxMaterial() = default;

	virtual void Initialize() override;
};