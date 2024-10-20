#pragma once
#include "RenderDefs.h"


class RTexture : public IRenderResource
{
protected:
	RTexture() = default;
	virtual ~RTexture() = default;
	inline BOOL IsDynamicTexture() const
	{
		return m_bIsDynamic;
	}

protected:
	DXGI_FORMAT m_format;
	BOOL m_bIsDynamic;
};


class RTexture2D : public RTexture
{
protected:
	RTexture2D() = default;
	~RTexture2D() = default;

	inline UINT GetWidth() const
	{
		return m_width;
	}

	inline UINT GetHeight() const
	{
		return m_height;
	}

protected:
	UINT m_width;
	UINT m_height;
};


class RTexture3D : public RTexture
{
protected:
	RTexture3D() = default;
	~RTexture3D() = default;

protected:
	UINT m_width;
	UINT m_height;
	UINT m_depth;
};

class RTextureCube : public RTexture
{
protected:
	RTextureCube() = default;
	~RTextureCube() = default;
};
