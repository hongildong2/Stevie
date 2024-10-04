#include "pch.h"
#include "RTexture2D.h"

RTexture2D::RTexture2D(const UINT width, const UINT height, const DXGI_FORMAT format)
	: IRenderResource()
	, m_width(width)
	, m_height(height)
	, m_format(format)
{
}

void RTexture2D::Initialize(IRenderer* pRenderer)
{
}
