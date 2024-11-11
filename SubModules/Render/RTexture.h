#pragma once
#include "RenderDefs.h"


enum class ETextureType
{
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_CUBE,
	STRUCTURED_BUFFER
};

class RTexture
	: public IRenderResource
{
public:
	inline BOOL IsReadOnly() const
	{
		return m_bIsReadOnly;
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
		MY_ASSERT(m_textureType == ETextureType::TEXTURE_3D);
		return m_depth;
	}

	inline ETextureType GetTextureType() const
	{
		return m_textureType;
	}


protected:
	RTexture(ETextureType type);
	virtual ~RTexture() = default;

protected:
	UINT m_width;
	UINT m_height;
	UINT m_depth;



protected:
	DXGI_FORMAT m_format;
	ETextureType m_textureType;
	BOOL m_bIsReadOnly;
	BOOL m_bInitialized;
};