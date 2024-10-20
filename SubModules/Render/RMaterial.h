#pragma once
#include "pch.h"
class IRenderer;
class RTexture;
class RTextureCube;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;



class RMaterial
{
public:
	RMaterial(const IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState);
	virtual ~RMaterial() = default;

	bool AddTexture(const RTexture* pTexture);


	virtual void Initialize();
	virtual void Update();

	void GetSamplerStates(void** ppOutSamplerStates, UINT* pOutSamplerStatesCount) const;
	void GetTextures(void** ppOutTextures, UINT* pOutTextureCount) const;

	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}
	inline const RBlendState* GetBlendState() const
	{
		return m_blendState;
	}

	inline const UINT GetSamplerStatesCount() const
	{
		return m_samplerStatesCount;
	}
	inline const UINT GetTexturesCount() const
	{
		return m_textureCount;
	}

protected:
	bool AddSamplerState(const RSamplerState* pSamplerState);
public:
	static constexpr UINT MATERIAL_TEXTURE_MAX_COUNT = 10;
	static constexpr UINT MATERIAL_SAMPLE_STATE_MAX_COUNT = 10;
	static constexpr UINT MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE = 512;


protected:
	const IRenderer* m_pRenderer;
	const RPixelShader* m_pixelShader;
	const RBlendState* m_blendState;

	const RSamplerState* m_samplerStates[MATERIAL_SAMPLE_STATE_MAX_COUNT];
	UINT m_samplerStatesCount;

	const RTexture* m_textures[MATERIAL_TEXTURE_MAX_COUNT];
	UINT m_textureCount;

	bool m_bInitialized;
};

struct RMaterialConstant
{
	UINT Data[128];
};

static_assert(sizeof(RMaterialConstant) <= RMaterial::MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE);


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