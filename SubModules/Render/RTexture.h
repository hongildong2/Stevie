#pragma once
#include "RenderDefs.h"


class RTexture : public IRenderResource
{
public:
	virtual BOOL IsDynamicTexture() = 0;
protected:
	const DXGI_FORMAT m_format;
};


class RTexture2D : public RTexture
{
public:
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
	const UINT m_width;
	const UINT m_height;
};


class RTexture3D final : public RTexture
{
public:
	RTexture3D(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format);
	~RTexture3D() = default;

private:
	const UINT m_width;
	const UINT m_height;
	const UINT m_depth;
};
