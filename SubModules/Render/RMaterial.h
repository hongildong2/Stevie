#pragma once
#include "pch.h"
class IRenderer;
class RTexture;
class RTextureCube;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;


struct RMaterialConstant
{
	UINT data[127];
	UINT size;

};
static_assert(sizeof(RMaterialConstant) % 16 == 0, "CONSTATN BUFFER ALIGNMENT");

class RMaterial
{
public:
	RMaterial(const IRenderer* pRenderer, const RPixelShader* pPixelShader, const RBlendState* pBlendState);
	virtual ~RMaterial() = default;

	bool AddTexture(const RTexture* pTexture);

	virtual void Initialize();
	virtual void Update();

	virtual void GetMaterialConstant(RMaterialConstant* pOutMaterialConstant) const = 0;
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
	static constexpr UINT MATERIAL_TEXTURE_MAX_COUNT = 20;
	static constexpr UINT MATERIAL_SAMPLE_STATE_MAX_COUNT = 10;
	static constexpr UINT MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE = 508;


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

static_assert(sizeof(RMaterialConstant) - 4 <= RMaterial::MATERIAL_CONSTANT_MAX_SIZE_IN_BYTE);


class RDemoMaterial final : public RMaterial
{
public:
	RDemoMaterial(const IRenderer* pRenderer);
	~RDemoMaterial() = default;

	virtual void GetMaterialConstant(RMaterialConstant* pOutMaterialConstant) const override;
};


class RSkyboxMaterial final : public RMaterial
{
public:
	RSkyboxMaterial(const IRenderer* pRenderer);
	~RSkyboxMaterial() = default;

	virtual void Initialize() override;
	virtual void GetMaterialConstant(RMaterialConstant* pOutMaterialConstant) const override;

};


struct RBasicMaterialConstant
{
	FLOAT metallicFactor;
	FLOAT aoFactor;
	FLOAT roughnessFactor;
	FLOAT t1;

	BOOL bUseTexture;
	DirectX::SimpleMath::Vector3 albedo;

	FLOAT metallic;
	FLOAT roughness;
	FLOAT specular;
	FLOAT IBLStrength;

	BOOL bUseHeightMap;
	FLOAT heightScale;
	DirectX::SimpleMath::Vector2 mcDummy;

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
	DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f),

	0.5f,
	0.5f,
	0.5f,
	1.f,

	FALSE,
	0.2f,
	DirectX::SimpleMath::Vector2(0.f, 0.f),

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
	RBasicMaterial(const IRenderer* pRenderer);
	~RBasicMaterial() = default;

	virtual void GetMaterialConstant(RMaterialConstant* pOutMaterialConstant) const override;
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