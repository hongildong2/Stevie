#include "pch.h"
#include "RTextureVolume.h"

RTextureVolume::RTextureVolume(const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format)
	: m_width(width)
	, m_height(height)
	, m_depth(depth)
	, m_format(format)
{
}
