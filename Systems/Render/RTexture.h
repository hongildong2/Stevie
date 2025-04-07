#pragma once
#include "pch.h"

// Texture :: SRV + TextureDepth :: DSV, TextureDynamic :: UAV, TextureRender : public TextureDynamic :: RTV, UAV


enum class ETextureType
{
	TEXTURE_2D,
	TEXTURE_2D_ARRAY,
	TEXTURE_2D_RENDER,
	TEXTURE_2D_DEPTH,
	TEXTURE_3D,
	TEXTURE_CUBE,
	STRUCTURED_BUFFER
};

#ifdef API_D3D11
#define RHITexture D3D11Texture
#include "Systems/Render/D3D11/D3D11Texture.h"
#endif


class RTexture : public RHITexture
{
	friend class RResourceManager;

#ifdef API_D3D11
#include "Systems/Render/D3D11/D3D11DeviceResources.h"
	friend class D3D11DeviceResources;
#endif

public:
	RTexture(ETextureType type, DXGI_FORMAT format, BOOL bIsDynamic);
	~RTexture() = default;

	inline virtual bool IsInitialized() const override
	{
		return m_bInitialized;
	}

	inline BOOL IsDynamic() const
	{
		return m_bIsDynamic;
	}

	inline ETextureType GetTextureType() const
	{
		return m_type;
	}

	inline UINT GetWidth() const
	{
		return m_width;
	}

	inline UINT GetHeight() const
	{
		return m_height;
	}

	inline UINT GetDepth() const
	{
		MY_ASSERT(m_type == ETextureType::TEXTURE_3D);
		return m_depth;
	}

	void SetSize(const UINT width, const UINT height, const UINT depth, const UINT count);
	void Initialize();

private:
	ETextureType m_type;
	DXGI_FORMAT m_format;
	BOOL m_bIsDynamic;
	BOOL m_bInitialized;

	UINT m_width; // Element size in structured buffer
	UINT m_height;
	UINT m_depth; // 3D
	UINT m_count; // Array or structured buffer
};


