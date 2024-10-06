#pragma once
#include "RenderDefs.h"
class RTextureVolume final : public IRenderResource
{
public:
	RTextureVolume(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format);
	~RTextureVolume() = default;

private:
	const UINT m_width;
	const UINT m_height;
	const UINT m_depth;
	const DXGI_FORMAT m_format;
};

