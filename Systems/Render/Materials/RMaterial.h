#pragma once
#include "pch.h"

class RRenderer;
class RTexture;
class RTextureCube;
class RShader;
class RSamplerState;
class RBlendState;
class RPixelShader;
struct RenderParam;


class RMaterial
{
public:
	RMaterial(RRenderer* pRenderer, const RPixelShader* pPixelShader);
	virtual ~RMaterial() = default;

	virtual void Initialize();
	virtual void Update();

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const = 0;

	void GetGeometryTextures(const RTexture** ppOutTextures) const;
	void GetGeometrySamplerStates(const RSamplerState** ppOutSamplerStates) const;

	void GetPixelTextures(const RTexture** ppOutTextures) const;
	void GetPixelSamplerStates(const RSamplerState** ppOutSamplerStates) const;

	inline const RPixelShader* GetShader() const
	{
		return m_pixelShader;
	}
	inline const BOOL IsHeightMapped() const
	{
		return m_bIsHeightMapped;
	}
	inline const UINT GetGeometryTexturesCount() const
	{
		return m_geometryTexturesCount;
	}
	inline const UINT GetGeometrySamplerStatesCount() const
	{
		return m_geometrySamplerStatesCount;
	}
	inline const UINT GetPixelTexturesCount() const
	{
		return m_pixelTexturesCount;
	}
	inline const UINT GetPixelSamplerStatesCount() const
	{
		return m_pixelSamplerStatesCount;
	}

protected:
	RRenderer* m_pRenderer;
	const RPixelShader* m_pixelShader;

	const RTexture* m_geometryTextures[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT m_geometryTexturesCount;
	const RSamplerState* m_geometrySamplerStates[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT m_geometrySamplerStatesCount;

	const RTexture* m_pixelTextures[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT m_pixelTexturesCount;
	const RSamplerState* m_pixelSamplerStates[renderLimits::MAX_RENDER_BINDINGS_COUNT];
	UINT m_pixelSamplerStatesCount;

	BOOL m_bInitialized;
	BOOL m_bIsHeightMapped;
};


// SkyboxMaterial
class RSkyboxMaterial final : public RMaterial
{
public:
	RSkyboxMaterial(RRenderer* pRenderer);
	~RSkyboxMaterial() = default;

	virtual void Initialize() override;
	void SetSkyboxTexture(const RTexture* pTex);

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;

};

// BasicMaterial
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
static_assert(sizeof(RBasicMaterialConstant) <= sizeof(RenderParam));

class RBasicMaterial final : public RMaterial
{
public:
	RBasicMaterial(RRenderer* pRenderer);
	~RBasicMaterial() = default;

	virtual void GetMaterialConstant(RenderParam* pOutRenderParam) const override;

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

	enum GEOMETRY_TEX_SLOTS
	{
		HEIGHT,
	};
	enum PIXEL_TEX_SLOTS
	{
		ALBEDO,
		AO,
		METALLIC,
		NORMAL,
		ROUGHNESS,
		EMISSIVE,
		OPACITY,
		COUNT
	};


	RBasicMaterialConstant m_constant;
};

