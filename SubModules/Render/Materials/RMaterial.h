#pragma once
#include "pch.h"

interface IRenderer;
class RTexture;
class RTextureCube;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;
struct RenderParam;


class RMaterial : public IRenderResource
{
public:
	RMaterial(IRenderer* pRenderer, const RPixelShader* pPixelShader);
	virtual ~RMaterial() = default;

	bool AddTexture(const RTexture* pTexture);

	virtual void Initialize();
	virtual void Update();

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const = 0;
	virtual void GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const = 0;

	void GetSamplerStates(const RSamplerState** ppOutSamplerStates, UINT* pOutSamplerStatesCount) const;
	virtual void GetTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const;

	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}
	inline const UINT GetSamplerStatesCount() const
	{
		return m_samplerStatesCount;
	}
	inline const UINT GetTexturesCount() const
	{
		return m_textureCount;
	}
	inline const BOOL IsHeightMapped() const
	{
		return m_bIsHeightMapped;
	}

protected:
	bool AddSamplerState(const RSamplerState* pSamplerState);

public:
	static constexpr UINT MATERIAL_TEXTURE_MAX_COUNT = 20;
	static constexpr UINT MATERIAL_SAMPLE_STATE_MAX_COUNT = 10;
	static constexpr UINT MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE = sizeof(RenderParam);


protected:
	IRenderer* m_pRenderer;
	const RPixelShader* m_pixelShader;

	const RSamplerState* m_samplerStates[MATERIAL_SAMPLE_STATE_MAX_COUNT];
	UINT m_samplerStatesCount;

	const RTexture* m_textures[MATERIAL_TEXTURE_MAX_COUNT];
	UINT m_textureCount;

	BOOL m_bInitialized;
	BOOL m_bIsHeightMapped;
};



class RDemoMaterial final : public RMaterial
{
public:
	RDemoMaterial(IRenderer* pRenderer);
	~RDemoMaterial() = default;

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;
	virtual void GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;

};

// SkyboxMaterial
class RSkyboxMaterial final : public RMaterial
{
public:
	RSkyboxMaterial(IRenderer* pRenderer);
	~RSkyboxMaterial() = default;

	virtual void Initialize() override;
	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;
	virtual void GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;

};

// BasicMaterial
struct RBasicMaterialConstant
{
	FLOAT metallicFactor;
	FLOAT aoFactor;
	FLOAT roughnessFactor;
	FLOAT t1;

	BOOL bUseTexture;
	Vector3 albedo;

	FLOAT metallic;
	FLOAT roughness;
	FLOAT specular;
	FLOAT IBLStrength;

	BOOL bUseHeightMap;
	FLOAT heightScale;
	Vector2 mcDummy;

	BOOL bUseAlbedoTexture;
	BOOL bUseAOTexture;
	BOOL bUseHeightTexture;
	BOOL bUseMetallicTexture;

	BOOL bUseNormalTexture;
	BOOL bUseRoughnessTexture;
	BOOL bUseEmissiveTexture;
	BOOL bUseOpacityTexture;
};

constexpr RBasicMaterialConstant DEFAULT_MATERIAL =
{
	0.7f,
	0.3f,
	1.f,
	1.f,

	TRUE,
	Vector3(1.f, 1.f, 1.f),

	0.5f,
	0.5f,
	0.5f,
	1.f,

	FALSE,
	0.2f,
	Vector2(0.f, 0.f),

	FALSE,
	FALSE,
	FALSE,
	FALSE,

	FALSE,
	FALSE,
	FALSE,
	FALSE
};
static_assert(sizeof(RBasicMaterialConstant) <= RMaterial::MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE);

class RBasicMaterial final : public RMaterial
{
public:
	RBasicMaterial(IRenderer* pRenderer);
	~RBasicMaterial() = default;

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;
	virtual void GetHeightMapTextures(const RTexture** ppOutTextures, UINT* pOutTextureCount) const override;

	void SetAlbedoTexture(const RTexture* pAlbedoTexture);
	void SetAOTexture(const RTexture* pAOTexture);
	void SetHeightTexture(const RTexture* pHeightTexture);
	void SetMetallicTexture(const RTexture* pMetallicTexture);
	void SetNormalTexture(const RTexture* pNormalTexture);
	void SetRoughnessTexture(const RTexture* pRoughnessTexture);
	void SetEmissiveTexture(const RTexture* pEmissiveTexture);
	void SetOpacityTexture(const RTexture* pOpacityTexture);

	virtual void Initialize() override;

private:
	enum BASIC_TEXTURE_INDEX
	{
		ALBEDO = 0,
		AO = 1,
		HEIGHT = 2,
		METALLIC = 3,
		NORMAL = 4,
		ROUGHNESS = 5,
		EMISSIVE = 6,
		OPACITY = 7,
		COUNT = 8
	};

	RBasicMaterialConstant m_constant;
};

