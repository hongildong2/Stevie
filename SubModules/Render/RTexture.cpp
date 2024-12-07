#include "pch.h"
#include "RTexture.h"

RTexture::RTexture(ETextureType type, DXGI_FORMAT format, BOOL bIsDynamic)
	: m_width(0)
	, m_height(0)
	, m_depth(0)
	, m_count(0)
	, m_format(format)
	, m_bIsDynamic(bIsDynamic)
	, m_bInitialized(FALSE)
	, m_type(type)
{
}

void RTexture::SetSize(const UINT width, const UINT height, const UINT depth, const UINT count)
{
	if (m_width == width && m_height == height && m_depth == depth && m_count == count)
	{
		return;
	}

	m_width = width;
	m_height = height;
	m_depth = depth;
	m_count = count;

	m_bInitialized = FALSE;
}
