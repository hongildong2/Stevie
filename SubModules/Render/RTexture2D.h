#pragma once
#include "IRenderer.h"
class RTexture2D : public IRenderResource
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
	const DXGI_FORMAT m_format;
};

